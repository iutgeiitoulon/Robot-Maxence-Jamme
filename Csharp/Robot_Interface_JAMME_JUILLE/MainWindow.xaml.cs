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
        Robot robot = new Robot();


        public MainWindow()
        {
            
            InitializeComponent();
            serialPort1 = new ReliableSerialPort("COM5", 115200, Parity.None, 8, StopBits.One);
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
            TextBoxReception.Background = Brushes.RoyalBlue;
        }

        private void Button_MouseLeave(object sender, MouseEventArgs e)
        {
            TextBoxReception.Background = Brushes.DarkRed;
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
                //…
                    if(c== 0xFE)
                    {
                        rcvState = StateReception.FunctionMSB;
                        //TextBoxReception.Text += "snideezszf";
                    }
                    break;
                case StateReception.FunctionMSB:
                    //…
                    msgDecodedFunction = (c<<8);
                    rcvState = StateReception.FunctionMSB;
                    break;
                case StateReception.FunctionLSB:
                    //…
                    msgDecodedFunction += c;
                    rcvState = StateReception.PayloadLengthMSB;
                    break;
                case StateReception.PayloadLengthMSB:
                    //…
                    msgDecodedPayloadLength = (c<<8);
                    rcvState = StateReception.PayloadLengthLSB;
                    break;
                case StateReception.PayloadLengthLSB:
                    //…
                    msgDecodedPayloadLength += c;
                    if (msgDecodedPayloadLength > 1500)
                    {
                        rcvState = StateReception.Waiting;
                    }
                    rcvState = StateReception.Payload;
                    break;
                case StateReception.Payload:
                    msgDecodedPayload[msgDecodedPayloadIndex] = c;
                    msgDecodedPayloadIndex++;
                    if(msgDecodedPayloadIndex == msgDecodedPayloadLength)
                    {
                        rcvState = StateReception.CheckSum;
                    }
                //…
                break;
                case StateReception.CheckSum:
                    //…
                    receivedChecksum = c;
                    calculatedChecksum = CalculateChecksum(msgDecodedFunction, msgDecodedPayloadLength, msgDecodedPayload);
                    if (calculatedChecksum == receivedChecksum){
                        TextBoxReception.Text += "youpi";
                    }
                    else
                    {
                        TextBoxReception.Text += "snif";
                    }
                    rcvState = StateReception.Waiting;
                    //…
                    break;
                default:
                    rcvState = StateReception.Waiting;
                break;
            }
        }
    }
}
