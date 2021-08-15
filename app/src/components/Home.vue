<template>
  <v-card max-width="500px" class="mx-auto mt-5">
    <v-container class="mb-0 pb-0">
      <v-row>
        <v-col class="text-center text-h6 font-weight-light">
          Configuration
        </v-col>
      </v-row>
      <v-row>
        <v-col>
          <v-expansion-panels flat>
            <v-expansion-panel>
              <v-expansion-panel-header
                color="secondary"
                class="white--text font-weight-light"
                >WiFi Config</v-expansion-panel-header
              >
              <v-expansion-panel-content>
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
                      color="secondary"
                      >save config on flash
                      <v-icon class="ml-2">{{ icons.chip }}</v-icon></v-btn
                    ></v-col
                  ></v-row
                >
              </v-expansion-panel-content>
            </v-expansion-panel>
            <v-expansion-panel>
              <v-expansion-panel-header
                color="secondary"
                class="white--text font-weight-light"
                >MQTT Config</v-expansion-panel-header
              >
              <v-expansion-panel-content>
                <v-form>
                  <v-row
                    ><v-col class="d-flex">
                      <v-text-field
                        v-model="mqttIp[0]"
                        class="mr-2"
                        required
                        type="number"
                        hint="MQTT IP Address"
                        persistent-hint
                      ></v-text-field>
                      <v-text-field
                        v-model="mqttIp[1]"
                        class="mx-1"
                        required
                        type="number"
                      ></v-text-field>
                      <v-text-field
                        v-model="mqttIp[2]"
                        class="mx-1"
                        required
                        type="number"
                      ></v-text-field>
                      <v-text-field
                        v-model="mqttIp[3]"
                        required
                        class="mx-1"
                        type="number"
                      ></v-text-field>
                    </v-col>
                    <v-col cols="12">
                      <v-text-field
                        v-model="mqttTopic"
                        label="MQTT Default topic"
                        required
                      ></v-text-field>
                      <v-text-field
                        v-model="mqttLogin"
                        label="MQTT username (optional)"
                        required
                      ></v-text-field>
                      <v-text-field
                        v-model="mqttPassword"
                        label="MQTT password (optional)"
                        required
                        type="password"
                      ></v-text-field>
                    </v-col>
                  </v-row>
                </v-form>
                <v-row
                  ><v-col class="text-center"
                    ><v-btn
                      @click="updateMqtt"
                      :loading="loading"
                      color="secondary"
                      >save config on flash
                      <v-icon class="ml-2">{{ icons.chip }}</v-icon></v-btn
                    ></v-col
                  ></v-row
                >
              </v-expansion-panel-content>
            </v-expansion-panel>
          </v-expansion-panels>
        </v-col>
      </v-row>
      <v-row>
        <v-fade-transition mode="out-in">
        <v-col cols="12" v-if="showError">
          <v-alert
            :color="color"
            class="white--text"
            border="left"
            mode="out-in"
            >{{ errorMessage }}</v-alert
          >
        </v-col>
        </v-fade-transition>

      </v-row>
    </v-container>
  </v-card>
</template>

<script lang="ts">
import Vue from "vue";
import Axios from "axios";
import { mdiChip } from "@mdi/js";

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
    icons: {
      chip: mdiChip,
    },
    ssid: "",
    password: "",
    mqttIp: [192, 168, 0, 0],
    mqttLogin: "",
    mqttPassword: "",
    mqttTopic: "weathermonitor",
    mqttLoading: false,
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
          "WiFi Configured! please rejoin your usual WiFi Connection and reboot your weather monitor";
        this.color = "success";
        this.showError = true;
        setTimeout(() => {
          this.showError = false;
        }, 10000);
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
    async updateMqtt() {
      // TBD
    },
  },
  async beforeMount() {
    try {
      const configObject = await axios.get("config");
      const config = configObject.data;
      this.ssid = config.ssid;
      this.password = config.password;
      this.mqttIp = config.mqttIp;
      this.mqttTopic = config.mqttTopic;
      this.mqttPassword = config.mqttPassword;
    } catch (error) {
      console.error(error);
    }
    // Trying to get the current config from the server
  },
});
</script>
