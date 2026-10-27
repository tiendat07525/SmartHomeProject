const mqtt = require('mqtt');
const { MongoClient } = require('mongodb');
const { InfluxDB, Point } = require('@influxdata/influxdb-client');

const MQTT_URL = process.env.MQTT_URL || 'mqtt://mqtt-broker:1883';
const INFLUX_URL = process.env.INFLUX_URL || 'http://influxdb:8086';
const INFLUX_TOKEN = process.env.INFLUX_TOKEN;
const INFLUX_ORG = process.env.INFLUX_ORG || 'iot_home';
const INFLUX_BUCKET = process.env.INFLUX_BUCKET || 'sensors_telemetry';
const MONGO_URI = process.env.MONGO_URI || 'mongodb://admin:secretpassword@mongodb:27017/iot_db?authSource=admin';

const GAS_ALERT_THRESHOLD = 1800;

async function run() {
  const mongoClient = new MongoClient(MONGO_URI);
  await mongoClient.connect();
  const db = mongoClient.db('iot_db');
  const eventsCollection = db.collection('system_events');
  console.log('[MongoDB] Đã kết nối cơ sở dữ liệu thành công.');

  const influx = new InfluxDB({ url: INFLUX_URL, token: INFLUX_TOKEN });
  const writeApi = influx.getWriteApi(INFLUX_ORG, INFLUX_BUCKET, 'ns', {
    batchSize: 10,
    flushInterval: 1000,
  });
  writeApi.useDefaultTags({ location: 'home' });

  const mqttClient = mqtt.connect(MQTT_URL);

  mqttClient.on('connect', () => {
    console.log('[Mosquitto] Kết nối MQTT Broker thành công.');
    mqttClient.subscribe([
      'smarthome/telemetry',
      'smarthome/mode',
      'smarthome/relay/control'
    ], (err) => {
      if (!err) {
        console.log('[Mosquitto] Đã subscribe các topic: telemetry, mode, relay/control');
      }
    });
  });

  mqttClient.on('error', (err) => {
    console.error('[Mosquitto Error]', err.message);
  });

  mqttClient.on('message', async (topic, message) => {
    const payloadStr = message.toString();

    if (topic === 'smarthome/telemetry') {
      try {
        const data = JSON.parse(payloadStr);
        const temp = !isNaN(data.temp) ? parseFloat(data.temp) : 0.0;
        const hum = !isNaN(data.hum) ? parseFloat(data.hum) : 0.0;
        const gas = parseInt(data.gas, 10) || 0;
        const lux = parseInt(data.lux, 10) || 0;
        const motion = data.motion ? 1 : 0;

        const point = new Point('environment')
          .tag('device', 'ESP32_SmartHome_Client')
          .floatField('temperature', temp)
          .floatField('humidity', hum)
          .intField('gas', gas)
          .intField('lux', lux)
          .intField('motion', motion);

        writeApi.writePoint(point);
        console.log('[InfluxDB] Ghi metric: T=' + temp + '°C, H=' + hum + '%, Gas=' + gas + ', Lux=' + lux + ', Motion=' + motion);

        if (gas > GAS_ALERT_THRESHOLD) {
          await eventsCollection.insertOne({
            type: 'GAS_LEAK_ALERT',
            level: 'CRITICAL',
            details: { gasValue: gas, threshold: GAS_ALERT_THRESHOLD },
            timestamp: new Date()
          });
          console.warn('[MongoDB Alert] Gas vượt ngưỡng: ' + gas + '!');
        }
      } catch (err) {
        console.error('[Error Telemetry JSON]', err.message);
      }
    } else if (topic === 'smarthome/mode' || topic === 'smarthome/relay/control') {
      try {
        const eventType = topic === 'smarthome/mode' ? 'MODE_CHANGE' : 'RELAY_CONTROL';
        await eventsCollection.insertOne({
          type: eventType,
          topic: topic,
          command: payloadStr,
          timestamp: new Date()
        });
        console.log('[MongoDB Event] Ghi nhận [' + eventType + ']: ' + payloadStr);
      } catch (err) {
        console.error('[Error Logging Event]', err.message);
      }
    }
  });

  process.on('SIGINT', async () => {
    await writeApi.close();
    await mongoClient.close();
    mqttClient.end();
    process.exit(0);
  });
}

run().catch(console.error);
