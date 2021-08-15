<template>
  <v-app>
    <v-app-bar app color="dark" dark class="d-flex justify-center">
      <div class="d-flex align-center">
        <div class="text-h6 font-weight-light text-center">
          Weather Monitor <v-icon class="ml-2">{{ weatherIcon }}</v-icon>
        </div>
      </div>
    </v-app-bar>
    <v-main>
      <Home />
    </v-main>
  </v-app>
</template>

<script lang="ts">
import Vue from "vue";
import Home from "./components/Home.vue";
import { mdiWeatherPartlyCloudy } from "@/assets/svg";

export default Vue.extend({
  name: "App",

  components: {
    Home,
  },

  data: () => ({
    //
    weatherIcon: mdiWeatherPartlyCloudy as string,
  }),
});
</script>
  <style lang="sass">

</style>