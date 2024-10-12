# Zephyr Example Application

- workspace application type (`app`) that showcases some basics
- navigate to the directory where you want to clone the repo and: `west init -m https://github.com/krystofh/zephyr-demo --mr main zephyr-demo`
  - if you installed west using Python virtual environment, make sure you activate it before like `source zephyr-demo/zephyr/.venv/bin/activate` 
- see [getting started guide](https://docs.zephyrproject.org/latest/develop/getting_started/index.html) which describes setup for `repository` applications/samples
- this example application follows T2 topology as described [here](https://docs.zephyrproject.org/latest/develop/getting_started/index.html)
- Build with `west build -b rpi_pico app` in the `zephyr-demo` subdirectory
- The `.uf2` file is located in `zephyr-demo/build/zephyr`

## Useful links

- Using USB serial communication https://www.gnd.io/zephyr-console-output/ 
  - Contrary to the article, one has to specify the overlay in `rpi_pico.overlay` as it has priority over `app.overlay`. Read more [here](https://docs.zephyrproject.org/3.3.0/build/dts/howtos.html#set-devicetree-overlays)
- Configure device as CDC (Communication device class) using ACM (abstract control module) https://docs.zephyrproject.org/latest/connectivity/usb/device/usb_device.html#cdc-acm