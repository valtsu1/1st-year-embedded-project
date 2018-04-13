/**
* @mainpage ZumoBot Project
* @brief    You can make your own ZumoBot with various sensors.
* @details  <br><br>
    <p>
    <B>General</B><br>
    You will use Pololu Zumo Shields for your robot project with CY8CKIT-059(PSoC 5LP) from Cypress semiconductor.This 
    library has basic methods of various sensors and communications so that you can make what you want with them. <br> 
    <br><br>
    </p>
    
    <p>
    <B>Sensors</B><br>
    &nbsp;Included: <br>
        &nbsp;&nbsp;&nbsp;&nbsp;LSM303D: Accelerometer & Magnetometer<br>
        &nbsp;&nbsp;&nbsp;&nbsp;L3GD20H: Gyroscope<br>
        &nbsp;&nbsp;&nbsp;&nbsp;Reflectance sensor<br>
        &nbsp;&nbsp;&nbsp;&nbsp;Motors
    &nbsp;Wii nunchuck<br>
    &nbsp;TSOP-2236: IR Receiver<br>
    &nbsp;HC-SR04: Ultrasonic sensor<br>
    &nbsp;APDS-9301: Ambient light sensor<br>
    &nbsp;IR LED <br><br><br>
    </p>
    
    <p>
    <B>Communication</B><br>
    I2C, UART, Serial<br>
    </p>
*/

#include <project.h>
#include <stdio.h>
#include "Systick.h"
#include "Motor.h"
#include "Ultra.h"
#include "Nunchuk.h"
#include "Reflectance.h"
#include "I2C_made.h"
#include "Gyro.h"
#include "Accel_magnet.h"
#include "IR.h"
#include "Ambient.h"
#include "Beep.h"
#include <time.h>
#include <sys/time.h>
int rread(void);

/**
 * @file    main.c
 * @brief   
 * @details  ** Enable global interrupt since Zumo library uses interrupts. **<br>&nbsp;&nbsp;&nbsp;CyGlobalIntEnable;<br>
*/
void forward(uint8 speed,uint32 delay)
{
    MotorDirLeft_Write(0);      // set LeftMotor forward mode
    MotorDirRight_Write(0);     // set RightMotor forward mode
    PWM_WriteCompare1(speed-6.7); 
    PWM_WriteCompare2(speed); 
    CyDelay(delay);
}

void counter(){
    
}

#if 0
//battery level//
int main()
{
    CyGlobalIntEnable; 
    UART_1_Start();
    Systick_Start();
    
    ADC_Battery_Start();        

    int16 adcresult = 0;
    float volts = 0.0;

    printf("\nBoot\n");
    int aika;
    //BatteryLed_Write(1); // Switch led on 
    //BatteryLed_Write(0); // Switch led off 
    //uint8 button;
    //button = SW1_Read(); // read SW1 on pSoC board
    // SW1_Read() returns zero when button is pressed
    // SW1_Read() returns one when button is not pressed

    aika = GetTicks();
    if (aika > 10000){
        BatteryLed_Write(0);
        ADC_Battery_StartConvert();
        if(ADC_Battery_IsEndConversion(ADC_Battery_WAIT_FOR_RESULT)) {   // wait for get ADC converted value
            adcresult = ADC_Battery_GetResult16(); // get the ADC value (0 - 4095)
            // convert value to Volts
            // you need to implement the conversion
            volts = ((adcresult / 4095.0) * 5)*1.5;
            // Print both ADC results and converted value
            printf("%d %f\r\n",adcresult, volts);
            if (volts < 4){
                BatteryLed_Write(1);
            }
        }
    aika = 0;  
    }
 }   
#endif

#if 0
// button
int main()
{
    CyGlobalIntEnable; 
    UART_1_Start();
    Systick_Start();
    
    printf("\nBoot\n");

    //BatteryLed_Write(1); // Switch led on 
    BatteryLed_Write(0); // Switch led off 
    
    //uint8 button;
    //button = SW1_Read(); // read SW1 on pSoC board
    // SW1_Read() returns zero when button is pressed
    // SW1_Read() returns one when button is not pressed
    
    bool led = false;
    
    for(;;)
    {
        // toggle led state when button is pressed
        if(SW1_Read() == 0) {
            led = !led;
            BatteryLed_Write(led);
            ShieldLed_Write(led);
            if(led) printf("Led is ON\n");
            else printf("Led is OFF\n");
            Beep(1000, 150);
            while(SW1_Read() == 0) CyDelay(10); // wait while button is being pressed
        }        
    }
 }   
#endif


#if 0
//ultrasonic sensor//
int main()
{
    CyGlobalIntEnable; 
    UART_1_Start();
    Systick_Start();
    Ultra_Start();                          // Ultra Sonic Start function
    while(1) {
        int d = Ultra_GetDistance();
        //If you want to print out the value  
        printf("distance = %d\r\n", d);
        CyDelay(200);
    }
}   
#endif


#if 0
//IR receiver//
int main()
{
    CyGlobalIntEnable; 
    UART_1_Start();
    IR_Start();
    
    uint32_t IR_val; 
    
    printf("\n\nIR test\n");
    
    IR_flush(); // clear IR receive buffer
    printf("Buffer cleared\n");
    
    IR_wait(); // wait for IR command
    printf("IR command received\n");
    
    // print received IR pulses and their lengths
    for(;;)
    {
        if(IR_get(&IR_val)) {
            int l = IR_val & IR_SIGNAL_MASK; // get pulse length
            int b = 0;
            if((IR_val & IR_SIGNAL_HIGH) != 0) b = 1; // get pulse state (0/1)
            printf("%d %d\r\n",b, l);
            //printf("%d %lu\r\n",IR_val & IR_SIGNAL_HIGH ? 1 : 0, (unsigned long) (IR_val & IR_SIGNAL_MASK));
        }
    }    
 }   
#endif

#if 1 
//reflectance//
int main()
{
    //struct sensors_ ref;
    struct sensors_ dig;
    
    Systick_Start();
    motor_start();
    motor_forward(0,100);

    CyGlobalIntEnable; 
    UART_1_Start();
    ADC_Battery_Start();        

    int16 adcresult = 0;
    float volts = 0.0;
    
    int suunta = 10;
    int stop = 0;
    long int time; 
    long int check = 10000;
  
    reflectance_start();
    reflectance_set_threshold(9000, 9000, 11000, 11000, 9000, 9000); // set center sensor threshold to 11000 and others to 9000

    for(;;)
    {
    
    time = GetTicks();
    
    //tarkastaa 10s välein onko pattereissa yli neljä volttia ja sytyttää ledin jos alle
    if (time > check){
        ADC_Battery_StartConvert();
        if(ADC_Battery_IsEndConversion(ADC_Battery_WAIT_FOR_RESULT)) {   // wait for get ADC converted value
            adcresult = ADC_Battery_GetResult16(); // get the ADC value (0 - 4095)
            // convert value to Volts
            // you need to implement the conversion
            volts = ((adcresult / 4095.0) * 5) * 1.5;

            if (volts < 4){
                BatteryLed_Write(1);
            }
        }
    check += time;  
    }
    


        //vasemman moottorin heitto on -6.7
        // read raw sensor values
        //reflectance_read(&ref);
        //printf("%5d %5d %5d %5d %5d %5d\r\n", ref.l3, ref.l2, ref.l1, ref.r1, ref.r2, ref.r3);       // print out each period of reflectance sensors
        
        // read digital values that are based on threshold. 0 = white, 1 = black
        // when blackness value is over threshold the sensors reads 1, otherwise 0
        reflectance_digital(&dig);      //print out 0 or 1 according to results of reflectance period
        //printf("%5d %5d %5d %5d %5d %5d \r\n", dig.l3, dig.l2, dig.l1, dig.r1, dig.r2, dig.r3);        //print out 0 or 1 according to results of reflectance period
        
        
        //jos kaikki sensorit näkevät mustaa, asettaa sunnaksi 0
        /*while ((dig.l3 == 1 && dig.r3 == 1)){
            forward(255,10);
            if((dig.l3 == 0 && dig.r3 == 0)) {
                stop++;
            }
        }*/

        //jos suunta 0 ja sensorit näkevät vain valkoista
        //else if ((dig.l1 == 0 && dig.l2 == 0 && dig.l3 == 0 && dig.r1 == 0 && dig.r2 == 0 && dig.r3 == 0) && suunta ==// 0){
        /*
        if((dig.l1 == 0 && dig.l2 == 0 && dig.l3 == 0 && dig.r1 == 0 && dig.r2 == 0 && dig.r3 == 0) && suunta == 1 && stop >= 3 && suunta == 0) {
            motor_forward(0,100);
        }*/
        
        //jos sensorit näkevät vain valkoista ja suunta 1
        if ((dig.l1 == 0 && dig.l2 == 0 && dig.l3 == 0 && dig.r1 == 0 && dig.r2 == 0 && dig.r3 == 0) && suunta == 1){
            motor_turn(0,255,5);
        }
        
        //jos sensorit näkevät vain valkoista ja suunta 2 
        else if ((dig.l1 == 0 && dig.l2 == 0 && dig.l3 == 0 && dig.r1 == 0 && dig.r2 == 0 && dig.r3 == 0) && suunta == 2){
            motor_turn(255,0,5);
        }
       //suoraan (asettaa suunnan arvoksi 0 ja mahdollistaa pysähtymisen valkoisella)
        else if((dig.l1 == 1 && dig.r1 == 1 && dig.l2 == 0 && dig.l3 == 0 && dig.r2 == 0 && dig.r3 == 0 )|| (dig.l1 == 1 && dig.r1 == 0 && dig.r2 == 0 && dig.r3 == 0 && dig.l2 == 0 && dig.l3 == 0) || (dig.r1 == 1 && dig.l1 == 0 && dig.r2 == 0 && dig.r3 == 0 && dig.l2 == 0 && dig.l3 == 0)) {
            forward(200,5);
            suunta = 0;
        }
        //jyrkkä vasen
        else if(dig.l3 == 1) {
                motor_turn(10,255,5);
                suunta = 1;
                
        }
        
        //jyrkkä oikea
        else if(dig.r3 == 1) {
            motor_turn(255,17,5);
            suunta = 2;
        }
        
        //vasemmalle (asettaa suunnan arvoksi 0 ja mahdollistaa pysähtymisen valkoisella)
       else if((dig.l2 == 1 && dig.l1 == 1 && dig.l3 == 0) || (dig.l2 == 1 && dig.l3 == 0 && dig.l1 == 0)) {
            motor_turn(180,200,10);
            suunta = 1;
        }
        
        //oikealle (asettaa suunnan arvoksi 0 ja mahdollistaa pysähtymisen valkoisella)
        else if((dig.r2 == 1 && dig.r1 == 1 && dig.r3 == 0) || (dig.r2 == 1 && dig.r1 == 0 && dig.r3 == 0)) {
            motor_turn(200,180,10);
            suunta = 2;
        }

        
        
    }
}   
#endif

#if 0
//reflectance//
int main()
{
    struct sensors_ ref;
    struct sensors_ dig;

    Systick_Start();
     motor_start();

    CyGlobalIntEnable; 
    UART_1_Start();
  
    reflectance_start();
    reflectance_set_threshold(9000, 9000, 11000, 11000, 9000, 9000); // set center sensor threshold to 11000 and others to 9000
    

    for(;;)
    {
        // read raw sensor values
        //reflectance_read(&ref);
        //printf("%5d %5d %5d %5d %5d %5d\r\n", ref.l3, ref.l2, ref.l1, ref.r1, ref.r2, ref.r3);       // print out each period of reflectance sensors
        
        // read digital values that are based on threshold. 0 = white, 1 = black
        // when blackness value is over threshold the sensors reads 1, otherwise 0
        reflectance_digital(&dig);      //print out 0 or 1 according to results of reflectance period
        //printf("%5d %5d %5d %5d %5d %5d \r\n", dig.l3, dig.l2, dig.l1, dig.r1, dig.r2, dig.r3);        //print out 0 or 1 according to results of reflectance period
        
        
        //stop
        if ((dig.l1 == 0 && dig.l2 == 0 && dig.l3 == 0 && dig.r1 == 0 && dig.r2 == 0 && dig.r3 == 0)){
            forward(0,100);
        }
       //suoraan
        else if((dig.l1 == 1 && dig.r1 == 1 && dig.l2 == 0 && dig.l3 == 0 && dig.r2 == 0 && dig.r3 == 0 )|| (dig.l1 == 1 && dig.r1 == 0 && dig.r2 == 0 && dig.r3 == 0 && dig.l2 == 0 && dig.l3 == 0) || (dig.r1 == 1 && dig.l1 == 0 && dig.r2 == 0 && dig.r3 == 0 && dig.l2 == 0 && dig.l3 == 0)) {
            forward(220,5);
        }
        //jyrkkä vasen
        else if(dig.l3 == 1) {
                motor_turn(0,220,5);
        }
        
        //jyrkkä oikea
        else if(dig.r3 == 1) {
            motor_turn(220,0,5);
        }
        
        //vasemmalle
       else if((dig.l2 == 1 && dig.l1 == 1 && dig.l3 == 0) || (dig.l2 == 1 && dig.l3 == 0 && dig.l1 == 0)) {
            motor_turn(160,180,5);
        }
        
        //oikealle
        else if((dig.r2 == 1 && dig.r1 == 1 && dig.r3 == 0) || (dig.r2 == 1 && dig.r1 == 0 && dig.r3 == 0)) {
            motor_turn(180,160,5);
        }
        
        
        
        
    }
}   
#endif


#if 0
//motor//
int main()
{
    CyGlobalIntEnable; 
    UART_1_Start();
    
    CyDelay(1500);
    
    motor_start();

    forward(255,1650);    
    motor_turn(150,0,750);   
    forward(255,1300); 
    motor_turn(150,0,750);   
    forward(255,1250);
    motor_turn(150,0,750);
    motor_turn(200,100,1800);
    forward(255,450);
           
    motor_stop();               // motor stop
    
    for(;;)
    {

    }
}
#endif


/* [] END OF FILE */
