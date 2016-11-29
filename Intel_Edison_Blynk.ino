#define BLYNK_PRINT Serial                  //开启串口调试
#include <WiFi.h>                           //关于Edison的WiFi的函数
#include <BlynkSimpleIntelEdisonWiFi.h>      //Blynk的关于Edison的函数封装
#include <SimpleTimer.h>                     //Arduino的定时器库
#include <Wire.h>                            //顾名思义就相应线的接口，比如说SPI、I2C等等
#include <rgb_lcd.h>                        //套件中的LCD库

char auth[] = "d68f5d77328c4c5c9e896c019d37ac1c";  //Blynk的Token，用来验证项目身份的

char ssid[] = "TP-LINK_C73EAC";                    //路由器名字
char pass[] = "wahahaha";                          //路由WiFi密码
char server_ip[] = "115.159.77.134";               //Blynk服务器地址
int port = 8442;                                   //Blynk服务器端口，默认的端口是8442

SimpleTimer timer;                                //定义一个定时器
rgb_lcd lcd;                                    //定义一个LCD对象

const int B = 3975;                      //用来计算我们的温度值
const int fanPin =  8;                   //定义一个继电器的引脚，控制轴流风机           
const int waterbumpPin = 4;              //这个是定义继电器控制水泵引脚
const int pinTemp = A0;                  //温度的模拟输入引脚
const int pinLight = A1;                 //光照强度的模拟输入引脚
const int pinEarth = A2;                 //土壤湿度的模拟输入引脚
 
WidgetLCD App_lcd(V3);                   //这个定义的是远程APP上的LCD，区别于本地的LCD

void setup()
{  
	Serial.begin(9600);   //开启串口调试
 
	lcd.begin(16, 2);     //初始化LCD
	lcd.print("Connect server...");  //LCD输出连接服务器信息
   
	Blynk.begin(auth, ssid, pass, server_ip, port); //连接我们的Blynk服务器
	
	while (Blynk.connect() == false) {
    // Wait until connected，等待连接成功
	}
    // 更新显示，表明系统已经连接到服务器
    lcd.clear();
    lcd.print("System Online!");
    
	//初始化轴流风机控制引脚，默认为低电平
    pinMode(fanPin, OUTPUT);                
    digitalWrite(fanPin, LOW);

	//初始化水泵控制引脚，默认为低电平
    pinMode(waterbumpPin, OUTPUT); 
    digitalWrite(waterbumpPin, LOW);
    
	//更新APP上面的显示，表明系统已经在线
	App_lcd.clear(); 
	App_lcd.print(0, 0, "System Online!"); // use: (position X: 0-15, position Y: 0-1, "Message you want to print")
		
	//定时器函数，定时1s钟执行一次time_event函数
	timer.setInterval(1000L, timer_event);
}

//定时器函数
void timer_event()
{
    // 得到温度信息的初始模拟值
    int val = analogRead(pinTemp);

    // 算出此时的温敏电阻的阻值
    float resistance = (float)(1023-val)*10000/val;

    //	通过温敏电阻的阻值算出实际的温度值
    float temperature = 1/(log(resistance/10000)/B+1/298.15)-273.15;

	//	更新Blynk的APP上面的V0引脚的值
    Blynk.virtualWrite(V0, temperature);
	
	//	获取光照强度的模拟值
	int sensorValue = analogRead(pinLight);
	
	//	更新我们的APP上面的V1的数据
    Blynk.virtualWrite(V1, sensorValue);

	// 得到我们的土壤湿度数据
    int earthValue = analogRead(pinEarth);

	// 更新我们的APP上面的V2的数据
    Blynk.virtualWrite(V2, earthValue);

}

void loop()
{
	Blynk.run();    //运行Blynk和服务器连接
	timer.run();    //定时器函数开始运行
}

