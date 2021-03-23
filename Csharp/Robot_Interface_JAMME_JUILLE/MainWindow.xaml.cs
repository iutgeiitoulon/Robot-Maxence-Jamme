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
        DispatcherTimer timerAffichage;
        public MainWindow()
        {
            InitializeComponent();
            serialPort1 = new ReliableSerialPort("COM3", 115200, Parity.None, 8, StopBits.One);
            serialPort1.DataReceived += SerialPort1_DataReceived;
            serialPort1.Open();

            timerAffichage = new DispatcherTimer();
            timerAffichage.Interval = new TimeSpan(0, 0, 0, 0, 100);
            timerAffichage.Tick += TimerAffichage_Tick;
            timerAffichage.Start();
        }

        private void TimerAffichage_Tick(object sender, EventArgs e)        // peut etre faut à voir
        {
            throw new NotImplementedException();
        }

        string receivedText;
        private void SerialPort1_DataReceived(object sender, DataReceivedArgs e)
        {
            TextBoxRéception.Text += receivedText;
        }
        
        void SendMessage()
        {
            string msg = TextBoxEmission.Text;
            serialPort1.WriteLine(msg);
            //TextBoxRéception.Text = TextBoxRéception.Text + "Reçu: " + msg;            
            TextBoxEmission.Text = "";
        }
        private void buttonEnvoyer_Click(object sender, RoutedEventArgs e)
        {
            string msg = TextBoxEmission.Text;
            TextBoxRéception.Text = TextBoxRéception.Text + "Reçu: " + msg + "\n";
            TextBoxEmission.Text = "";
        }

        private void TextBoxEmission_KeyUp(object sender, KeyEventArgs e)
        
        {
            if (e.Key == Key.Enter)
            {
                SendMessage();
            }
        }
    }
}
