using ExtendedSerialPort;
using System;
using System.Collections.Generic;
using System.IO.Ports;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;
using System.Windows.Threading;
using System.Threading.Tasks;
using System.Threading;

namespace Robot_Interface_JAMME_JUILLE
{
    /// <summary>
    /// Logique d'interaction pour MainWindow.xaml
    /// </summary>

    
    public partial class MainWindow : Window
    {
        ReliableSerialPort serialPort1;
        AsyncCallback SerialPort1_DataRecived;
        DispatcherTimer timerAffichage;

        int i;
        int couleur;
        int couleur_2 = 0;
        Robot robot = new Robot();


        public MainWindow()
        {
            
            InitializeComponent();
            serialPort1 = new ReliableSerialPort("COM4", 115200, Parity.None, 8, StopBits.One);
            serialPort1.DataReceived += SerialPort1_DataReceived;
            serialPort1.Open();

            timerAffichage = new DispatcherTimer();
            timerAffichage.Interval = new TimeSpan(0, 0, 0, 0, 100);
            timerAffichage.Tick += TimerAffichage_Tick;
            timerAffichage.Start();
        }

        private void TimerAffichage_Tick(object sender, EventArgs e)        // peut etre faut à voir
        {
            /*if (robot.receivedText != "")
            {
                TextBoxReception.Text = TextBoxReception.Text + "Reçu=" + robot.receivedText;
                robot.receivedText = "";
            }*/
            while (robot.byteListReceived.Count != 0)
            {
                byte byteReceived = robot.byteListReceived.Dequeue();
                DecodeMessage(byteReceived);
                //string blabla;
                //blabla = byteReceived.ToString("X");
                //blabla += ;
                //TextBoxReception.Text += "0x" + byteReceived.ToString("2X") + "(" + Convert.ToChar(byteReceived) + ")\n"; // "0x"+blabla+"";
                //TextBoxReception.Text += byteReceived+"\n";
                //TextBoxReception.Text += Convert.ToChar(byteReceived);
            }
            
        }
        
        byte CalculateChecksum (int msgFunction, int msgPayLoadLength, byte[] msgPayLoad)
        {
            byte Checksum = 0;
            Checksum ^= (byte)(0xFE);
            Checksum ^= (byte)(msgFunction >> 8);
            Checksum ^= (byte)(msgFunction >> 0);
            Checksum ^= (byte)(msgPayLoadLength >> 8);
            Checksum ^= (byte)(msgPayLoadLength >> 0);
            for (int i = 0; i < msgPayLoadLength; i++)
            {
                Checksum ^= (byte)msgPayLoad[i];
            }            
            return Checksum;
        }

        void UartEncodeAndSendMessage(int msgFunction , int msgPayloadLength , byte[] msgPayload)
        {
            byte Checksum = 0;
            
            byte[] trame = new byte[msgPayloadLength+6];
            int pos = 0;
            trame[pos++] = (byte)(0xFE);
            trame[pos++] = (byte)(msgFunction >> 8);
            trame[pos++] = (byte)(msgFunction >> 0);
            trame[pos++] = (byte)(msgPayloadLength >> 8);
            trame[pos++] = (byte)(msgPayloadLength >> 0);
            for (int i = 0; i < msgPayloadLength; i++)
            {
                trame[pos++] = (byte) msgPayload[i];
            }
            Checksum = CalculateChecksum(msgFunction, msgPayloadLength, msgPayload);
            
            trame[pos++] = (byte)(Checksum);
            serialPort1.Write(trame, 0, trame.Length);
        }

        private void SerialPort1_DataReceived(object sender, DataReceivedArgs e)
        {
            //robot.receivedText += Encoding.UTF8.GetString(e.Data, 0, e.Data.Length);
            for (i = 0; i < e.Data.Length; i++)
            {
                robot.byteListReceived.Enqueue(e.Data[i]);
            }

        }

        void SendMessage()
        {
            serialPort1.WriteLine(TextBoxEmission.Text);
            TextBoxEmission.Text = "";
        }
        private void buttonEnvoyer_Click(object sender, RoutedEventArgs e)
        {
            SendMessage();
        }

        private void TextBoxEmission_KeyUp(object sender, KeyEventArgs e)

        {
            if (e.Key == Key.Enter)
            {
                SendMessage();
            }
        }

        private void Button_Click(object sender, RoutedEventArgs e)
        {
            TextBoxReception.Text = "";
        }

        private void Button_MouseEnter(object sender, MouseEventArgs e)
        {
            if (couleur_2 == 1)
            {
                switch (couleur)
                {
                    case 0:
                        TextBoxReception.Background = (SolidColorBrush)new BrushConverter().ConvertFromString("#FF0000");
                        couleur++;
                        break;
                    case 1:
                        TextBoxReception.Background = (SolidColorBrush)new BrushConverter().ConvertFromString("#00FF00");
                        couleur++;
                        break;
                    case 2:
                        TextBoxReception.Background = (SolidColorBrush)new BrushConverter().ConvertFromString("#0000FF");
                        couleur = 0;
                        break;

                }
            }
            
            //TextBoxReception.Background = Brushes.RoyalBlue;
        }

        private void Button_MouseLeave(object sender, MouseEventArgs e)
        {
            if (couleur_2 == 1)
            {
                TextBoxReception.Background = (SolidColorBrush)new BrushConverter().ConvertFromString("#150057");
            }
            else
            {
                TextBoxReception.Background = (SolidColorBrush)new BrushConverter().ConvertFromString("#c9c9c9");
            }
            
        }

        private void Button_Click_1(object sender, RoutedEventArgs e)
        {

            /*byte[] byteList = new byte[20];
             for (int i = 0; i < 20; i++)
             {
                 byteList[i] = (byte)(2 * i);
             }
             serialPort1.Write(byteList, 0, byteList.Length);*/

            byte[] msgPayload = Encoding.ASCII.GetBytes(TextBoxEmission.Text);
            int msgFunction= 0x0080;
            int msgPayloadLength= msgPayload.Length;
            UartEncodeAndSendMessage(msgFunction, msgPayloadLength, msgPayload);
            TextBoxEmission.Text = "";
        }
        public enum StateReception
        {
            Waiting,
            FunctionMSB,
            FunctionLSB,
            PayloadLengthMSB,
            PayloadLengthLSB,
            Payload,
            CheckSum
        }

        StateReception rcvState = StateReception.Waiting;
        int msgDecodedFunction = 0;
        int msgDecodedPayloadLength = 0;
        byte[] msgDecodedPayload;
        int msgDecodedPayloadIndex = 0;
        byte receivedChecksum = 0;
        byte calculatedChecksum = 0;
        private void DecodeMessage(byte c)
        {
            switch (rcvState)
            {
                case StateReception.Waiting:
                    if(c== 0xFE)
                    {
                        rcvState = StateReception.FunctionMSB;
                    }
                    break;
                case StateReception.FunctionMSB:
                    msgDecodedFunction = (c<<8);
                    rcvState = StateReception.FunctionLSB;
                    break;
                case StateReception.FunctionLSB:
                    msgDecodedFunction += c;
                    rcvState = StateReception.PayloadLengthMSB;
                    break;
                case StateReception.PayloadLengthMSB:
                    msgDecodedPayloadLength = (c<<8);
                    rcvState = StateReception.PayloadLengthLSB;
                    break;
                case StateReception.PayloadLengthLSB:
                    msgDecodedPayloadLength += c;
                    if (msgDecodedPayloadLength > 1500)
                    {
                        rcvState = StateReception.Waiting;
                    }
                    rcvState = StateReception.Payload;
                    msgDecodedPayload = new byte[msgDecodedPayloadLength];
                    break;
                case StateReception.Payload:
                    msgDecodedPayload[msgDecodedPayloadIndex] = c;
                    msgDecodedPayloadIndex++;
                    if(msgDecodedPayloadIndex == msgDecodedPayloadLength)
                    {
                        rcvState = StateReception.CheckSum;
                    }                
                break;
                case StateReception.CheckSum:
                    receivedChecksum = c;
                    calculatedChecksum = CalculateChecksum(msgDecodedFunction, msgDecodedPayloadLength, msgDecodedPayload);
                    if (calculatedChecksum == receivedChecksum){
                        TextBoxReception.Text += "youpi\n";
                        ProcessDecodedMessage(msgDecodedFunction, msgDecodedPayloadLength, msgDecodedPayload);
                    }
                    else
                    {
                        TextBoxReception.Text += "snif";
                    }
                    rcvState = StateReception.Waiting;
                    break;
                default:
                    rcvState = StateReception.Waiting;
                break;
            }
        }

        public enum FunctionId
        {
            text = 0x0080,
            led = 0x0020,
            telem = 0x0030,
            vitesse = 0x0040,
        }
        //FunctionId fid = FunctionId.text;

        void ProcessDecodedMessage(int msgFunction,int msgPayloadLength, byte[] msgPayload)
        {
            if (msgFunction == (int)FunctionId.text)
            {                
                TextBoxReception.Text += "0x" + msgFunction.ToString("X4") + "\n";
                TextBoxReception.Text += msgPayloadLength + "\n";
                for (i = 0; i< msgPayloadLength; i++)
                {
                    TextBoxReception.Text += Convert.ToChar(msgPayload[i]);
                }
                TextBoxReception.Text += "\n";
            }
        }

        private void checkBox_Click(object sender, RoutedEventArgs e)
        {
            byte[] msgPayload;
            int msgFunction = (int)FunctionId.led;
            if (checkBox.IsChecked == true)
            {
                //send on
                TextTest.Text += "ON1";
                msgPayload = Encoding.ASCII.GetBytes("1");
            }
            else
            {
                //send off
                TextTest.Text += "OFF1";
                msgPayload = Encoding.ASCII.GetBytes("2");
            }
            int msgPayloadLength = msgPayload.Length;
            UartEncodeAndSendMessage(msgFunction, msgPayloadLength, msgPayload);
        }

        private void checkBox1_Click(object sender, RoutedEventArgs e)
        {
            int msgFunction = (int)FunctionId.led;
            if (checkBox1.IsChecked == true)
            {
                //send on
                TextTest.Text += "ON2";
            }
            else
            {
                //send off
                TextTest.Text += "OFF2";
            }
        }

        private void checkBox2_Click(object sender, RoutedEventArgs e)
        {
            int msgFunction = (int)FunctionId.led;
            if (checkBox2.IsChecked == true)
            {
                //send on
                TextTest.Text += "ON3";
            }
            else
            {
                //send off
                TextTest.Text += "OFF3";
            }
        }

        private void Button_Click_2(object sender, RoutedEventArgs e)
        {
            //textBox3.Text = "100";
        }

        private void TextTest_MouseEnter(object sender, MouseEventArgs e)
        {
            TextTest.Text = "";
        }

        private void Button_Click_3(object sender, RoutedEventArgs e)
        {
            if(couleur_2 == 0)
            {
                Background = (SolidColorBrush)new BrushConverter().ConvertFromString("#4CFF37");

                GB1.Foreground = (SolidColorBrush)new BrushConverter().ConvertFromString("#FC37FF");
                GB1.Background = (SolidColorBrush)new BrushConverter().ConvertFromString("#B200FF");
                TextBoxEmission.Background = (SolidColorBrush)new BrushConverter().ConvertFromString("#0B4DB2");

                GB2.Foreground = (SolidColorBrush)new BrushConverter().ConvertFromString("#A20007");
                GB2.Background = (SolidColorBrush)new BrushConverter().ConvertFromString("#FF00AE");
                TextBoxReception.Background = (SolidColorBrush)new BrushConverter().ConvertFromString("#150057");

                GB3.Foreground = (SolidColorBrush)new BrushConverter().ConvertFromString("#B0835D");
                GB3.Background = (SolidColorBrush)new BrushConverter().ConvertFromString("#00E8FF");
                TextTest.Background = (SolidColorBrush)new BrushConverter().ConvertFromString("#0B2747");

                BT1.Background = (SolidColorBrush)new BrushConverter().ConvertFromString("#9C88D9");
                BT2.Background = (SolidColorBrush)new BrushConverter().ConvertFromString("#0D8A35");
                BT3.Background = (SolidColorBrush)new BrushConverter().ConvertFromString("#EA6B7D");
                BT4.Background = (SolidColorBrush)new BrushConverter().ConvertFromString("#FCB400");
                BT5.Background = (SolidColorBrush)new BrushConverter().ConvertFromString("#BFFC00");
                BT1.Foreground = (SolidColorBrush)new BrushConverter().ConvertFromString("#B2A50B");
                BT2.Foreground = (SolidColorBrush)new BrushConverter().ConvertFromString("#2807BD");
                BT3.Foreground = (SolidColorBrush)new BrushConverter().ConvertFromString("#079FBD");
                BT4.Foreground = (SolidColorBrush)new BrushConverter().ConvertFromString("#000C3B");
                BT5.Foreground = (SolidColorBrush)new BrushConverter().ConvertFromString("#CE0D0D");

                checkBox.Background = (SolidColorBrush)new BrushConverter().ConvertFromString("#43A08B");
                checkBox1.Background = (SolidColorBrush)new BrushConverter().ConvertFromString("#FFC322");
                checkBox2.Background = (SolidColorBrush)new BrushConverter().ConvertFromString("#120754");
                checkBox.Foreground = (SolidColorBrush)new BrushConverter().ConvertFromString("#C914A3");
                checkBox1.Foreground = (SolidColorBrush)new BrushConverter().ConvertFromString("#50CCF4");
                checkBox2.Foreground = (SolidColorBrush)new BrushConverter().ConvertFromString("#344730");

                textBox.Background = (SolidColorBrush)new BrushConverter().ConvertFromString("#2F4285");
                textBox1.Background = (SolidColorBrush)new BrushConverter().ConvertFromString("#901C02");
                textBox2.Background = (SolidColorBrush)new BrushConverter().ConvertFromString("#2BA612");
                textBox3.Background = (SolidColorBrush)new BrushConverter().ConvertFromString("#6A4D00");
                textBox4.Background = (SolidColorBrush)new BrushConverter().ConvertFromString("#6712A6");

                couleur_2 = 1;
            }
            else
            {
                Background = (SolidColorBrush)new BrushConverter().ConvertFromString("#FFFFFF");

                GB1.Foreground = (SolidColorBrush)new BrushConverter().ConvertFromString("#000000");
                GB1.Background = (SolidColorBrush)new BrushConverter().ConvertFromString("#c9c9c9");
                TextBoxEmission.Background = (SolidColorBrush)new BrushConverter().ConvertFromString("#c9c9c9");

                GB2.Foreground = (SolidColorBrush)new BrushConverter().ConvertFromString("#000000");
                GB2.Background = (SolidColorBrush)new BrushConverter().ConvertFromString("#c9c9c9");
                TextBoxReception.Background = (SolidColorBrush)new BrushConverter().ConvertFromString("#c9c9c9");

                GB3.Foreground = (SolidColorBrush)new BrushConverter().ConvertFromString("#000000");
                GB3.Background = (SolidColorBrush)new BrushConverter().ConvertFromString("#c9c9c9");
                TextTest.Background = (SolidColorBrush)new BrushConverter().ConvertFromString("#c9c9c9");

                BT1.Background = (SolidColorBrush)new BrushConverter().ConvertFromString("#DDDDDD");
                BT2.Background = (SolidColorBrush)new BrushConverter().ConvertFromString("#DDDDDD");
                BT3.Background = (SolidColorBrush)new BrushConverter().ConvertFromString("#DDDDDD");
                BT4.Background = (SolidColorBrush)new BrushConverter().ConvertFromString("#DDDDDD");
                BT5.Background = (SolidColorBrush)new BrushConverter().ConvertFromString("#DDDDDD");
                BT1.Foreground = (SolidColorBrush)new BrushConverter().ConvertFromString("#000000");
                BT2.Foreground = (SolidColorBrush)new BrushConverter().ConvertFromString("#000000");
                BT3.Foreground = (SolidColorBrush)new BrushConverter().ConvertFromString("#000000");
                BT4.Foreground = (SolidColorBrush)new BrushConverter().ConvertFromString("#000000");
                BT5.Foreground = (SolidColorBrush)new BrushConverter().ConvertFromString("#000000");

                checkBox.Background = (SolidColorBrush)new BrushConverter().ConvertFromString("#FFFFFF");
                checkBox1.Background = (SolidColorBrush)new BrushConverter().ConvertFromString("#FFFFFF");
                checkBox2.Background = (SolidColorBrush)new BrushConverter().ConvertFromString("#FFFFFF");
                checkBox.Foreground = (SolidColorBrush)new BrushConverter().ConvertFromString("#000000");
                checkBox1.Foreground = (SolidColorBrush)new BrushConverter().ConvertFromString("#000000");
                checkBox2.Foreground = (SolidColorBrush)new BrushConverter().ConvertFromString("#000000");

                textBox.Background = (SolidColorBrush)new BrushConverter().ConvertFromString("#FFFFFF");
                textBox1.Background = (SolidColorBrush)new BrushConverter().ConvertFromString("#FFFFFF");
                textBox2.Background = (SolidColorBrush)new BrushConverter().ConvertFromString("#FFFFFF");
                textBox3.Background = (SolidColorBrush)new BrushConverter().ConvertFromString("#FFFFFF");
                textBox4.Background = (SolidColorBrush)new BrushConverter().ConvertFromString("#FFFFFF");

                couleur_2 = 0;
            }
        }
    }
}
