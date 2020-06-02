# How To Add a Customised Lv2 Driver

1. Add a dedicated folder for your own customised Lv2 driver.
2. Add a driver.h inside the folder, and name a SWITCH macro and update "osa_hal/driver/customised/driver.h" to pointer to the driver.h inside your own folder when the macro is detected.
3. do what you want inside your folder but not the outside world.



