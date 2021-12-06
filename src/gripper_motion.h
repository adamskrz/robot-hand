// extern unsigned long closingTime;

// Setup function (runs when Arduino is powered on or reset)
void gripper_setup();

int getForce();

bool gripperClosed();

bool gripperHolding();
void stopGripper();

// void setGripperMotor(bool closing_dir, uint8_t speed = 0);

unsigned long calibrate_gripper();
void closeGripper();

double gripper_width(unsigned long currentTime, unsigned long fullTime = 0);