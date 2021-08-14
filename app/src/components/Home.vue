<template>
  <v-card max-width="500px" class="mx-auto mt-5">
    <v-container>
      <v-row>
        <v-col class="text-center text-h6 font-weight-light">
          WiFi configurator
        </v-col>
      </v-row>
      <v-form ref="wifiForm" class="text-center">
        <v-row class="d-flex justify-center">
          <v-col cols="12">
            <v-text-field
              v-model="ssid"
              label="SSID (2.4GHz)"
              required
            ></v-text-field>
            <v-text-field
              v-model="password"
              label="Password"
              required
              type="password"
            ></v-text-field>
          </v-col>
        </v-row>
      </v-form>
      <v-row
        ><v-col class="text-center"
          ><v-btn
            @click="updateFlash"
            :loading="loading"
            :disabled="!buttonEnabled"
            color="success"
            >save config on flash <v-icon class="ml-2">mdi-chip</v-icon></v-btn
          ></v-col
        ></v-row
      >
      <v-row>
        <v-col cols="12">
          <v-alert
            v-model="showError"
            :color="color"
            border="left"
            dismissible
            transition="fade-transition"
            mode="out-in"
            >{{ errorMessage }}</v-alert
          >
        </v-col>
      </v-row>
    </v-container>
  </v-card>
</template>

<script lang="ts">
import Vue from "vue";
import Axios from "axios";

const axios = Axios.create({
  baseURL: "http://192.168.8.18",
  timeout: 10000,
});

export default Vue.extend({
  name: "HelloWorld",

  computed: {
    buttonEnabled() {
      return this.ssid.length && this.password.length > 7;
    },
  },
  data: () => ({
    ssid: "",
    password: "",
    loading: false,
    showError: false,
    errorMessage: "An error occured, please try again",
    color: "warning",
  }),
  methods: {
    async updateFlash() {
      try {
        this.loading = true;
        await axios.post(
          "update",
          `ssid=${this.ssid}&password=${this.password}`
        );
        this.errorMessage =
          "WiFi Configured! System will reboot, please rejoin your usual WiFi Connection";
        this.color = "success";
        this.showError = true;
        setTimeout(() => {
          this.showError = false;
        }, 3000);
        this.loading = false;
      } catch (error) {
        this.errorMessage = "An error occured, please try again";
        this.showError = true;
        this.loading = false;
        setTimeout(() => {
          this.showError = false;
        }, 3000);
      }
    },
  },
});
</script>
