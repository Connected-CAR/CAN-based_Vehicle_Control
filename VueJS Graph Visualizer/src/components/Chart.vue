<script setup>
import { ref } from "vue";
import io from "socket.io-client"
import VueApexCharts from "vue3-apexcharts";
import dayjs from "dayjs";

const times = ref([]);
const temperatures = ref([]);
const humidities = ref([]);
const lights = ref([]);
const options = ref({});
const series = ref([]);
const perceived = ref([]);
const gpt_series = ref([]);
const gpt_options = ref([]);

const props = defineProps({
  msg: String,
})

const socket = io("http://13.124.218.110:8080");
socket.on("kfc", (arg) => {
  console.log(arg);
  times.value = arg.map((x) => dayjs(x.time).format("HH:mm:ss"));
  temperatures.value = arg.map((x) => x.temperature);
  humidities.value = arg.map((x) => x.humidity);
  lights.value = arg.map((x) => x.light);
  options.value = {
    xaxis: {
      categories: times.value,
    },
  };
  series.value = [
    {
      name: "온도",
      data: temperatures.value,
    },
    {
      name: "습도",
      data: humidities.value,
    },
    {
      name: "대기압",
      data: lights.value,
    },
  ];
});

socket.on("chatgpt", (arg) => {
  console.log(arg);
  times.value = arg.map((x) => dayjs(x.time).format("HH:mm:ss"));
  perceived.value = arg.map((x) => x.perceived_tem);
  gpt_options.value = {
    xaxis: {
      categories: times.value,
    },
  };
  gpt_series.value = [
    {
      name: "체감 온도",
      data: perceived.value,
    },
  ];
});

socket.emit("bbq", "is soso");
</script>

<template>
  <div class="container">
    <h1>차량 환경 변화 그래프</h1>
    <h3>
      온도/습도/조도
    </h3>
    <div class="graph">
        <VueApexCharts width="500" type="line" :options="options" :series="series" />
        <VueApexCharts width="500" type="line" :options="gpt_options" :series="gpt_series" />
    </div>
  </div>
</template>

<style>
    .container {display : flex; justify_content : center; align-items : center; flex-direction : column;}
</style>