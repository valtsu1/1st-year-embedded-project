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
#include <stdlib.h>
#include <time.h>
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

//otti huomioon moottorien nopeuserot, käytettiin silloin kun robotilla ajeltiin ilman sensoreita
void forward(uint8 speed,uint32 delay)
{
    MotorDirLeft_Write(0);      // set LeftMotor forward mode
    MotorDirRight_Write(0);     // set RightMotor forward mode
    PWM_WriteCompare1(speed-6.7); 
    PWM_WriteCompare2(speed); 
    CyDelay(delay);
}

//mustien viivojen laskuri
int lines(int left, int right) {
    static int check;
    static int fakeBoolean;
    
    //asettaa arvoksi 1 kun robotti on mustan viivan päällä
    if(left == 1 && right == 1) {
        fakeBoolean = 1;
    }
    
    //jos robotti on ollut mustan viivan päällä ja näkee taas valkoista lisää checkin arvoa yhdellä
    if(fakeBoolean == 1 && left == 0 && right == 0) {
        check++;
        fakeBoolean = 0;
    }
    return check;
}

//laskee keskiarvon kolmesta sensorin lukuarvosta
int avarage(float avarage) {
    if (avarage/3 >  0.6) {
        return 1;
    }
    else {
        return 0;
    }
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
    Ultra_Start();                          // Ultra Sonic Start function
    while(1) {
        int d = Ultra_GetDistance();
        //If you want to print out the value  
        printf("distance = %d\r\n", d);
        CyDelay(200);
}
}
    
#endif


#if 1
int main()
{
    CyGlobalIntEnable;
    UART_1_Start();
    motor_start();
    Systick_Start();
    srand(time(NULL)); 
    //Ultra_Start();                          // Ultra Sonic Start function
    //while(1) {
        //int d = Ultra_GetDistance();
        //If you want to print out the value  
        //printf("distance = %d\r\n", d);
        //CyDelay(200)
    struct sensors_ dig;
    int start = 0;
    reflectance_start();
    reflectance_set_threshold(14000,14000,14000,14000,14000,14000);
     while(start != 1) {
        reflectance_digital(&dig);
        CyDelay(1);
        motor_forward(50,5);
        if(dig.l3 == 1 && dig.r3 == 1) {
            start = 1;
        }
    }
    
    //viivalle ajanut robotti jää odottamaan kaukosäätimen aloituskomentoa
    motor_forward(0,100);
    IR_Start();
    Ultra_Start();                          // Ultra Sonic Start function
    IR_flush();
    IR_wait();
    motor_forward(255,300);
    for(;;){
        reflectance_digital(&dig);
        int first_random = rand() % 2;
        int second_random = rand() % 300 + 200;
        int distance = Ultra_GetDistance();
        if ((distance <= 35 && dig.r3 == 0 && dig.l3 == 0)  || (dig.r3 == 0 && dig.l3 == 0)) {
            MotorDirLeft_Write(0);
            MotorDirRight_Write(0);
            forward(255,2);
        }
        
        else{
            motor_backward(200,250);
                
            //vasen käännös
            if (first_random == 0){
                MotorDirLeft_Write(1);      // set LeftMotor backward mode
                MotorDirRight_Write(0);     // set RightMotor backward mode
                for (int i = 0; i<8;i++){
                    motor_turn(255,255,second_random / 8);
                    distance = Ultra_GetDistance();
                    if(distance < 35){
                        break;
                    }
                }
            }
            //oikea käännös
            else{
                MotorDirLeft_Write(0);      // set LeftMotor backward mode
                MotorDirRight_Write(1);     // set RightMotor backward mode
                for (int i = 0; i<8;i++){
                    motor_turn(255,255,second_random / 8);
                    distance = Ultra_GetDistance();
                    if(distance < 35){
                        break;
                    }
            }   
        }
    }
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

#if 0
//reflectance//
int main()
{
    //koodipätkiä joita saatetaan tarvita testaamiseen
    //------------------------------------------------
    //printf("%5d %5d %5d %5d %5d %5d \r\n", dig.l3, dig.l2, dig.l1, dig.r1, dig.r2, dig.r3);
    //printf("%5d %5d %5d %5d %5d %5d\r\n", ref.l3, ref.l2, ref.l1, ref.r1, ref.r2, ref.r3);
    //reflectance_set_threshold(15000,15000, 15000, 15000, 15000, 15000); 
    //------------------------------------------------
    
    struct sensors_ ref;
    struct sensors_ dig;
    
    //kalibrointiin käytettävät min/max arvot
    int l1min = 10000;
    int l1max = 10000;
    int l2min = 10000;
    int l2max = 10000;
    int l3min = 10000;
    int l3max = 10000;
    int r1min = 10000;
    int r1max = 10000;
    int r2min = 10000;
    int r2max = 10000;
    int r3min = 10000;
    int r3max = 10000;
    
    //kalibroinnin lopullinen arvo
    int l1value,l2value,l3value;
    int r1value,r2value,r3value;
    
    //muuttujat joilla lasketaan keskiarvo mustalle/valkoiselle, tarkoitus eliminoida virhearvot
    float left1,left2,left3 = 0;
    float right1,right2,right3 = 0;
    
    Systick_Start();
    motor_start();
    motor_forward(0,100);
    IR_Start();


    CyGlobalIntEnable; 
    UART_1_Start();
    ADC_Battery_Start();        
    
    //patterin jännitteenseuraamiseen käytettävät muuttujat
    int16 adcresult = 0;
    float volts = 0.0;
    long int time = 0;
    long int check = 10000;
    
    //arvo joka kertoo mihin robotti on viimeksi kääntynyt
    int suunta = 8;
    
    //robotin mustan viivan laskuri
    int stop = 0;
    
    //muutuja jota käytetään aloitusviivan löytämiseen
    int start = 0;
    
    reflectance_start();
    
    for(;;) {
            if(SW1_Read() == 0) {
                suunta = 10;
                break;
            }
    }
    
    CyDelay(1000);
    
    //kalibrointi loop, jota ajetaan niin kauan että robotin keskinappia painetaan
    for(;;) {

        reflectance_read(&ref);
            
        if(ref.l1 < l1min && ref.l1 > 0) {
            l1min = ref.l1;
        }
        if(ref.l1 > l1max) {
            l1max = ref.l1;
        }
        if(ref.l2 < l2min && ref.l2 > 0) {
            l2min = ref.l2;
        }
        if(ref.l2 > l2max) {
            l2max = ref.l2;
        }
        if(ref.l3 <= l3min && ref.l3 > 0) {
            l3min = ref.l3;
        }
        if(ref.l3 > l3max) {
            l3max = ref.l3;
        }
        if(ref.r1 < r1min && ref.r1 > 0) {
            r1min = ref.r1;
        }
        if(ref.r1 > r1max) {
            r1max = ref.r1;
        }
        if(ref.r2 < r2min && ref.r2 > 0) {
            r2min = ref.r2;
        }
        if(ref.r2 > r2max) {
            r2max = ref.r2;
        }
        if(ref.r3 < r3min && ref.r3 > 0) {
            r3min = ref.r3;
        }
        if(r3max < ref.r3) {
            r3max = ref.r3;
        }
        
        
        
        //lopettaa kalibroinnin jos robotin keskinappia painetaan    
        if(suunta == 14) {
            motor_forward(0,100);
            BatteryLed_Write(1);
            CyDelay(1000);
            BatteryLed_Write(0);
            break;
            
        }
        
        if(suunta == 13) {
            MotorDirLeft_Write(1);    
            MotorDirRight_Write(0);    
            PWM_WriteCompare1(100); 
            PWM_WriteCompare2(100); 
            if(ref.r1 > 16000 && ref.l1 > 16000) {
            suunta = 14;
            }
        }
        
        //oikeelle vol2
        if(suunta == 12) {
            MotorDirLeft_Write(0);    
            MotorDirRight_Write(1);    
            PWM_WriteCompare1(100); 
            PWM_WriteCompare2(100); 
            if(ref.r3 > 20000 && ref.r1 < 10000 && ref.r2 < 10000 && ref.l1 < 10000 && ref.l2 < 10000 && ref.l3 < 10000) {
            suunta = 13;
            }
        }
        
        
        //oikeelle
        if(suunta == 11) {
            MotorDirLeft_Write(0);    
            MotorDirRight_Write(1);    
            PWM_WriteCompare1(100); 
            PWM_WriteCompare2(100); 
            if(ref.l3 > 20000 && ref.l1 < 10000 && ref.l2 < 10000 && ref.r1 < 10000 && ref.r2 < 10000 && ref.r3 < 10000) {
                suunta = 12;
            }
        }
        
        //vasemmalle
        if(suunta == 10) {
            MotorDirLeft_Write(1);    
            MotorDirRight_Write(0);    
            PWM_WriteCompare1(100); 
            PWM_WriteCompare2(100); 
            if(ref.l3 > 20000) {
            suunta = 11;
            }
        }
        CyDelay(1);
    }


    
        
            //laskee ja asettaa kalibroinnin tuloksen perusteella arvot sensoreille
            l1value = ((l1min + l1max) / 3);
            l2value = ((l2min + l2max) / 3);
            l3value = ((l3min + l3max) / 3);
            
            r1value = ((r1min + r1max) / 3);
            r2value = ((r2min + r2max) / 3);
            r3value = ((r3min + r3max) / 3);
            
            reflectance_set_threshold(l3value,l2value,l1value,r1value,r2value,r3value);
    
    
    //robotti lähtee ajamaan aloitusviivalle, robotti ajaa suoraan niin kauan kunnes reunimmaiset sensorit näkevät mustaa
    while(start != 1) {
        reflectance_digital(&dig);
        CyDelay(1);
        motor_forward(50,5);
        if(dig.l3 == 1 && dig.r3 == 1) {
            start = 1;
        }
    }
    
    //viivalle ajanut robotti jää odottamaan kaukosäätimen aloituskomentoa
    motor_forward(0,100);
    IR_flush();
    IR_wait();
    motor_forward(255,5);
    
    
    //viivanseuraus loop
    for(;;)
    {
    
    //asettaa molemmat moottorit menemään suoraan
    MotorDirLeft_Write(0);      
    MotorDirRight_Write(0);
    
    //alustaa sensorien keskiarvotulokset
    left1 = 0;
    left2 = 0;
    left3 = 0;
        
    right1 = 0;
    right2 = 0;
    right3 = 0;
        
       
    //tarkastaa 10s välein onko pattereissa yli neljä volttia ja sytyttää ledin jos alle
    if (time > check){
        Beep(50,255);
        ADC_Battery_StartConvert();
        
        if(ADC_Battery_IsEndConversion(ADC_Battery_WAIT_FOR_RESULT)) {   
            adcresult = ADC_Battery_GetResult16(); 
            volts = (adcresult / 4095.0) * 5 * 1.5;

            if (volts < 4){
                BatteryLed_Write(1);
                Beep(50,255);
                Beep(100,001);
                Beep(50,255);
            }
        }
    check += time;  
    }
    time = GetTicks();


        //lukee sensorit kolme kertaa ja laskee niille keskiarvon 
        for(int i=0;i<=2;i++) {
            reflectance_digital(&dig); 
            left1+=dig.l1;
            left2+=dig.l2;
            left3+=dig.l3;
            right1+=dig.r1;
            right2+=dig.r2;
            right3+=dig.r3;
        }
        
        left1 = avarage(left1);
        left2 = avarage(left2);
        left3 = avarage(left3);
        right1 = avarage(right1);
        right2 = avarage(right2);
        right3 = avarage(right3);
        
        
        //laskuri joka nousee yhdellä aina kun robotti menee mustan viivan yli
        stop = lines(dig.l3, dig.r3);
        
       
        //kolmannen viivan ylitettyään, robotti pysähtyy
        if(stop >= 3) {
            motor_stop();
            Beep(200,245);
            Beep(200,001);
            break;
        }
        
        //jos sensorit näkevät vain valkoista ja suunta 1 (robotti jatkaa vasemmalle kääntymistä vaikka hukkaa mustan viivan)
        if ((left1 == 0 && left2 == 0 && left3 == 0 && right1 == 0 && right2 == 0 && right3 == 0) && suunta == 1){
            MotorDirLeft_Write(1);    
            MotorDirRight_Write(0);    
            PWM_WriteCompare1(255); 
            PWM_WriteCompare2(255); 
            CyDelay(1);
        }
        
        //jos sensorit näkevät vain valkoista ja suunta 2 (robotti jatkaa oikealle kääntymistä vaikka hukkaa mustan viivan)
        else if ((left1 == 0 && left2 == 0 && left3 == 0 && right1 == 0 && right2 == 0 && right3 == 0) && suunta == 2){
            MotorDirLeft_Write(0);      
            MotorDirRight_Write(1);     
            PWM_WriteCompare1(255); 
            PWM_WriteCompare2(255); 
            CyDelay(1);
        }
        
       //robotti ajaa suoraan
        else if((left1 == 1 && right1 == 1 && left2 == 0 && left3 == 0 && right2 == 0 && right3 == 0 )|| (left1 == 1 && right1 == 0 && right2 == 0 && right3 == 0 && left2 == 0 && left3 == 0) || (right1 == 1 && left1 == 0 && right2 == 0 && right3 == 0 && left2 == 0 && left3 == 0)) {
            forward(255,5);
        }
        
        //jyrkkä vasen
        else if(left3 == 1) {
            motor_turn(20,255,1);
            suunta = 1;       
        }
        
        //jyrkkä oikea
        else if(right3 == 1) {
            motor_turn(255,20,1);
            suunta = 2;
        }
        
       //loiva vasen
       else if((left2 == 1 && left1 == 1 && left3 == 0) || (left2 == 1 && left3 == 0 && left1 == 0)) {
            motor_turn(244,255,1);
            suunta = 1;
        }
        
        //loiva oikea
        else if((right2 == 1 && right1 == 1 && right3 == 0) || (right2 == 1 && right1 == 0 && right3 == 0)) {
            motor_turn(255,250,1);
            suunta = 2;
        } 
        CyDelay(1);
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
