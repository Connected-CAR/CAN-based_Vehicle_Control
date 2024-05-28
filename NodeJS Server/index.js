const express = require('express')
const app = express()
const port = 8080
const cors = require("cors");
const pool = require("./db");

const http = require('http')
const server = http.createServer(app)
const { Server } = require('socket.io');
const io = new Server(server, {
  cors:{
    origin: true
  },
  pingInterval: 100, //100 ms
  pingTimeout: 1
});

app.use(
    cors({
        origin: true
    })
);
app.get('/sensor', async (req, res) => {
  const ret = await pool.query("select * from sensing order by time desc limit 15");
  res.send(ret[0]); 
})

app.get('/sensor/temperature', async (req, res) => {
  const ret = await pool.query("select temperature, time from sensing");
  res.send(ret[0]);
})

app.get('/sensor/temperature/1', async (req, res) => {
  const ret = await pool.query("select temperature, time from sensing order by time desc limit 1");
  res.send(ret[0]);
})

app.get('/sensor/humidity', async (req, res) => {
  const ret = await pool.query("select humidity, time from sensing");
  res.send(ret[0]);
})

app.get('/sensor/humidity/1', async (req, res) => {
  const ret = await pool.query("select humidity, time from sensing order by time desc limit 1");
  res.send(ret[0]);
})

app.get('/sensor/light', async (req, res) => {
  const ret = await pool.query("select light, time from sensing");
  res.send(ret[0]);
})

app.get('/sensor/light/1', async (req, res) => {
  const ret = await pool.query("select light, time from sensing order by time desc limit 1");
  res.send(ret[0]);
})

app.get('/rccar', async (req, res) => {
  const ret = await pool.query("select * from command where is_finish = 0 order by time limit 1");
  res.send(ret[0]); 
})

app.get('/rccar/all', async (req, res) => {
  const ret = await pool.query("select * from command order by time");
  res.send(ret[0]); 
})

app.get("/rccar/finished/:id", async (req, res) => {
  const ret = await pool.query("update command set is_finish = 1 where id = ?", [Number(req.params.id)]);
  res.send(ret[0]);
})



io.on("connection", async (socket) => {
  const ret = await pool.query("select * from sensing order by time desc limit 15");
  socket.emit("kfc", ret[0]);

  socket.on("bbq", (arg) => {
    console.log(arg);
  })

  const ret1 = await pool.query("select * from chat_gpt order by time desc limit 15");
  socket.emit("chatgpt", ret1[0]);

  socket.on("chatgpt123", (arg) => {
    console.log(arg);
  })
})


server.listen(port, () => {
  console.log(`Example app listening on port ${port}`)
})
