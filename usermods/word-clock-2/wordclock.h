#pragma once

#include "wled.h"
#include "wordframe.h"

/*
 * Usermods allow you to add own functionality to WLED more easily
 * See: https://github.com/Aircoookie/WLED/wiki/Add-own-functionality
 * 
 * This usermod drives an 13x11 pixel matrix wordclock with WLED.
 *
 * There are 2 parameters to chnage the behaviour:
 * 
 * active: enable/disable usermod
 * word color (RRGGBB): color of the letters
 */
class WordClock2Usermod : public Usermod 
{
  private:

    static const uint32_t DEFAULT_WORD_COLOR = 0x05CBFF;

    // set your config variables to their boot default value (this can also be done in readFromConfig() or a constructor if you prefer)
    bool config_wordclock_active_ = true;
    uint32_t config_word_color_ = DEFAULT_WORD_COLOR;

    WordFrame word_frame_;

    String colorToHexString(uint32_t c)
    {
        char buffer[9];
        sprintf(buffer, "%06X", c);
        return buffer;
    }

    bool hexStringToColor(String const& s, uint32_t& c, uint32_t def)
    {
        char *ep;
        unsigned long long r = strtoull(s.c_str(), &ep, 16);
        if (*ep == 0) {
            c = r;
            return true;
        } else {
            c = def;
            return false;
        }
    }

  public:
    /*
     * setup() is called once at boot. WiFi is not yet connected at this point.
     * You can use it to initialize variables, sensors or similar.
     */
    void setup()
    {
    }

    /*
     * connected() is called every time the WiFi is (re)connected
     * Use it to initialize network interfaces
     */
    void connected()
    {
    }

    /*
     * loop() is called continuously. Here you can check for events, read sensors, etc.
     * 
     * Tips:
     * 1. You can use "if (WLED_CONNECTED)" to check for a successful network connection.
     *    Additionally, "if (WLED_MQTT_CONNECTED)" is available to check for a connection to an MQTT broker.
     * 
     * 2. Try to avoid using the delay() function. NEVER use delays longer than 10 milliseconds.
     *    Instead, use a timer check as shown here.
     */
    void loop()
    {
      static unsigned long last_time = 0;
      static int last_time_minutes = -1;

      // do it every second
      if (millis() - last_time > 1000)
      {
        uint8_t m = minute(localTime);

        // check if we already updated this minute
        if (last_time_minutes != m)
        {
          uint8_t h = hour(localTime) % 12;

          word_frame_.fromTime(h, m);

          last_time_minutes = m;
        }

        // remember last update
        last_time = millis();
      }
    }

     /*
     * handleOverlayDraw() is called just before every show() (LED strip update frame) after effects have set the colors.
     * Use this to blank out some LEDs or set them to a different color regardless of the set effect mode.
     * Commonly used for custom clocks (Cronixie, 7 segment)
     */
    void handleOverlayDraw() override
    {
      if (config_wordclock_active_)
        for (int y = 0; y < word_frame_.HEIGHT; y++)
          for (int x = 0; x < word_frame_.WIDTH; x++)
            if (word_frame_.isSet(x, y))
              strip.setPixelColorXY(x, y, config_word_color_);
    }

    /*
     * addToConfig() can be used to add custom persistent settings to the cfg.json file in the "um" (usermod) object.
     * It will be called by WLED when settings are actually saved (for example, LED settings are saved)
     * If you want to force saving the current state, use serializeConfig() in your loop().
     * 
     * CAUTION: serializeConfig() will initiate a filesystem write operation.
     * It might cause the LEDs to stutter and will cause flash wear if called too often.
     * Use it sparingly and always in the loop, never in network callbacks!
     * 
     * addToConfig() will make your settings editable through the Usermod Settings page automatically.
     *
     * Usermod Settings Overview:
     * - Numeric values are treated as floats in the browser.
     *   - If the numeric value entered into the browser contains a decimal point, it will be parsed as a C float
     *     before being returned to the Usermod.  The float data type has only 6-7 decimal digits of precision, and
     *     doubles are not supported, numbers will be rounded to the nearest float value when being parsed.
     *     The range accepted by the input field is +/- 1.175494351e-38 to +/- 3.402823466e+38.
     *   - If the numeric value entered into the browser doesn't contain a decimal point, it will be parsed as a
     *     C int32_t (range: -2147483648 to 2147483647) before being returned to the usermod.
     *     Overflows or underflows are truncated to the max/min value for an int32_t, and again truncated to the type
     *     used in the Usermod when reading the value from ArduinoJson.
     * - Pin values can be treated differently from an integer value by using the key name "pin"
     *   - "pin" can contain a single or array of integer values
     *   - On the Usermod Settings page there is simple checking for pin conflicts and warnings for special pins
     *     - Red color indicates a conflict.  Yellow color indicates a pin with a warning (e.g. an input-only pin)
     *   - Tip: use int8_t to store the pin value in the Usermod, so a -1 value (pin not set) can be used
     *
     * See usermod_v2_auto_save.h for an example that saves Flash space by reusing ArduinoJson key name strings
     * 
     * If you need a dedicated settings page with custom layout for your Usermod, that takes a lot more work.  
     * You will have to add the setting to the HTML, xml.cpp and set.cpp manually.
     * See the WLED Soundreactive fork (code and wiki) for reference.  https://github.com/atuline/WLED
     * 
     * I highly recommend checking out the basics of ArduinoJson serialization and deserialization in order to use custom settings!
     */
    void addToConfig(JsonObject& root) override
    {
      JsonObject top = root.createNestedObject("Wordclock");
      top["active"] = config_wordclock_active_;
      top["word color (RRGGBB)"] = colorToHexString(config_word_color_);
    }

    /*
     * readFromConfig() can be used to read back the custom settings you added with addToConfig().
     * This is called by WLED when settings are loaded (currently this only happens immediately after boot, or after saving on the Usermod Settings page)
     * 
     * readFromConfig() is called BEFORE setup(). This means you can use your persistent values in setup() (e.g. pin assignments, buffer sizes),
     * but also that if you want to write persistent values to a dynamic buffer, you'd need to allocate it here instead of in setup.
     * If you don't know what that is, don't fret. It most likely doesn't affect your use case :)
     * 
     * Return true in case the config values returned from Usermod Settings were complete, or false if you'd like WLED to save your defaults to disk (so any missing values are editable in Usermod Settings)
     * 
     * getJsonValue() returns false if the value is missing, or copies the value into the variable provided and returns true if the value is present
     * The configComplete variable is true only if the "exampleUsermod" object and all values are present.  If any values are missing, WLED will know to call addToConfig() to save them
     * 
     * This function is guaranteed to be called on boot, but could also be called every time settings are updated
     */
    bool readFromConfig(JsonObject& root) override
    {
      JsonObject top = root["Wordclock"];
      bool configComplete = !top.isNull();
      String color;
      configComplete &= getJsonValue(top["active"], config_wordclock_active_);
      configComplete &= getJsonValue(top["word color (RRGGBB)"], color, "005CBFF") && hexStringToColor(color, config_word_color_, DEFAULT_WORD_COLOR);
      return configComplete;
    }

    /*
     * getId() allows you to optionally give your V2 usermod an unique ID (please define it in const.h!).
     * This could be used in the future for the system to determine whether your usermod is installed.
     */
    uint16_t getId() override
    {
      return USERMOD_ID_WORDCLOCK2;
    }

};
