# Zephyr RTOS Example Application

Showcase of Zephyr RTOS functionality such as:

- usage of the `shell` module and `CLI` commands
- setting GPIO output (built-in LED or externally connected one)
- button press detection using `gpio_callback` (not shown in [demonstration](#demonstration) yet) and controlling the LED state with it
- device tree overlay definitions to match used hardware
- usage of `prj.conf` and `Kconfig` for enabling necessary Zephyr libraries and setting up user-defined project settings (analog to macros)
- usage of logging (information, error messages, ...)
- sending messages manually per commands into message queue of a defined size, to FIFO and to linked list
- running a producer and consumer thread for a (mockup) sensor reading every x seconds

This example application follows the T2 topology (workspace application type `app`) as described [here](https://docs.zephyrproject.org/latest/develop/getting_started/index.html).

Tested using:
- Raspberry Pi Pico 1 (RP 2040)
- macOS Sequoia 15.0.1
- VS Code Version: 1.94.2

# Build

- navigate to the directory where you want to clone the repo and: 
  - ```bash
    west init -m https://github.com/krystofh/zephyr-demo --mr main zephyr-demo
    ```
  - if you installed west using Python virtual environment, make sure you activate it before like `source zephyr-demo/zephyr/.venv/bin/activate` 
- see [getting started guide](https://docs.zephyrproject.org/latest/develop/getting_started/index.html) which describes setup for `repository` applications/samples
- Build with `west build -b rpi_pico app` in the `zephyr-demo` subdirectory
  - use `-t menuconfig` for customization of project configuration
  - use `-p always` for a clean build (pristine directory)
- The `.uf2` file is located in `zephyr-demo/build/zephyr`

# Available commands

After connecting to the device, following list of commands is available. Use `help` for more details.

```bash
Available commands:
  clear  : Clear screen.
  device  : Device commands
  devmem  : Read/write physical memory
               Usage:
               Read memory at address with optional width:
               devmem <address> [<width>]
               Write memory at address with mandatory width and value:
               devmem <address> <width> <value>
  help  : Prints the help message.
  history  : Command history.
  kernel  : Kernel commands
  led_blink  : Blink the LED with a defined period in ms.
  led_off  : Turn the LED OFF
  led_on  : Turn the LED ON
  params  : Print params command.
  ping  : Respond with pong
  read_fifo  : Read first (oldest) message from the FIFO queue
  read_ll  : Read message from the linked list
  read_msgq  : Read first (oldest) message from MSGQ
  rem  : Ignore lines beginning with 'rem '
  resize  : Console gets terminal screen size or assumes default in case the
               readout fails. It must be executed after each terminal width
               change to ensure correct text display.
  retval  : Print return value of most recent command
  send_fifo  : Send a message to FIFO message queue
  send_ll  : Send a message to linked list
  send_msgq  : Send a message to MSGQ
  shell  : Useful, not Unix-like shell commands.
```

# Demonstration

![CLI commands demo](doc/showcase.gif)

![CLI commands demo](doc/showcase_video.gif)

## Useful links and hints

- When using the shell, set line ending to `CR`
- Using USB serial communication https://www.gnd.io/zephyr-console-output/ 
  - Contrary to the article, one has to specify the overlay in `rpi_pico.overlay` as it has priority over `app.overlay`. Read more [here](https://docs.zephyrproject.org/3.3.0/build/dts/howtos.html#set-devicetree-overlays)
- Configure device as CDC (Communication device class) using ACM (abstract control module) https://docs.zephyrproject.org/latest/connectivity/usb/device/usb_device.html#cdc-acm
- Building the `shell_module` with USB console using snippet instead of project configuration, use command: `west build -b rpi_pico -S cdc-acm-console -p always zephyr/samples/subsys/shell/shell_module`
- Youtube tutorial on Zephyr RTOS: https://youtube.com/playlist?list=PLAymxPbYHgl-FZSggEx_lRJoPU2h4tT36&si=HKek54uZ2aw1xCJK
- GPIOs: https://docs.zephyrproject.org/latest/hardware/peripherals/gpio.html
- Workqueues: https://docs.zephyrproject.org/latest/kernel/services/threads/workqueue.html#workqueue-threads
- Message queues: https://docs.zephyrproject.org/latest/kernel/services/data_passing/message_queues.html
- FIFOs: https://docs.zephyrproject.org/latest/kernel/services/data_passing/fifos.html
- Single-linked list: https://docs.zephyrproject.org/latest/kernel/data_structures/slist.html
- Random numbers: https://docs.zephyrproject.org/apidoc/latest/group__random__api.html