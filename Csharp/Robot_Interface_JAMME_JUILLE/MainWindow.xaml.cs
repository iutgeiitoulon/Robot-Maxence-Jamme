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
            string blabla;
            blabla = byteReceived.ToString();
            TextBoxReception.Text += blabla;
            }
            
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
            
            byte[] byteList = new byte[20];
            for (int i = 0; i < 20; i++)
            {
                byteList[i] = (byte)(2 * i);
            }
            serialPort1.Write(byteList, 0, byteList.Length);
        }
    }
}
