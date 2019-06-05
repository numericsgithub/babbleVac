using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Drawing;
using System.Data;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using System.IO;
using System.Diagnostics;
using System.Management;
using System.Collections.Concurrent;
using System.Threading;

namespace BabbleVacSDClient.Controls
{
    public partial class Step3Control : StepBaseControl
    {
        private string currentStatus = "";
        private bool quickFormat;
        private ConcurrentQueue<string> errors = new ConcurrentQueue<string>();
        public char DriveLetter { get; private set; }

        public Step3Control(char driveLetter, bool quickFormat)
        {
            InitializeComponent();
            this.DriveLetter = driveLetter;
            this.quickFormat = quickFormat;
        }

        public void Start()
        {
            backgroundWorker.RunWorkerAsync();
            timer.Start();
            textBox1.Text += "Errors will be displayed here ...\r\n\r\n";
        }

        private static bool FormatDrive_CommandLine(ref string status, char driveLetter, string label = "", string fileSystem = "FAT32", bool quickFormat = true, bool enableCompression = false, int? clusterSize = null)
        {
            #region args check

            //if (!Char.IsLetter(driveLetter) ||
            //    !IsFileSystemValid(fileSystem))
            //{
            //    return false;
            //}

            #endregion
            bool success = false;
            string drive = driveLetter + ":";
            try
            {
                var di = new DriveInfo(drive);
                var psi = new ProcessStartInfo();
                psi.FileName = "format.com";
                psi.CreateNoWindow = true; //if you want to hide the window
                psi.WorkingDirectory = Environment.SystemDirectory;
                psi.Arguments = "/FS:" + fileSystem +
                                             " /Y" +
                                             " /V:" + label +
                                             (quickFormat ? " /Q" : "") +
                                             //((fileSystem == "NTFS" && enableCompression) ? " /C" : "") +
                                             //(clusterSize.HasValue ? " /A:" + clusterSize.Value : "") +
                                             " " + drive;
                psi.UseShellExecute = false;
                psi.CreateNoWindow = true;
                psi.RedirectStandardOutput = true;
                psi.RedirectStandardInput = true;
                var formatProcess = Process.Start(psi);
                var swStandardInput = formatProcess.StandardInput;
                swStandardInput.WriteLine();
                string s = "";
                while (!formatProcess.HasExited)
                {
                    while (formatProcess.StandardOutput.EndOfStream && !formatProcess.HasExited)
                        Thread.Sleep(1);
                    if (formatProcess.HasExited)
                        break;
                    s += (char)formatProcess.StandardOutput.Read();
                    if(s.Length > 400)
                        s = s.Remove(0, s.Length - 400);
                    string[] lines = s.Split('\r');
                    if(lines.Length < 2)
                        status = s;
                    else status = "Currently formatting drive (can take up to 30 minutes): " + lines[lines.Length - 2];
                }
                formatProcess.WaitForExit();
                success = true;
            }
            catch (Exception ex) { MessageBox.Show("Error while formatting: " + ex.Message); }
            return success;
        }

        private static void FormatProcess_OutputDataReceived(object sender, DataReceivedEventArgs e)
        {
            MessageBox.Show(e.Data);
        }

        private void Timer_Tick(object sender, EventArgs e)
        {
            lProcess.Text = currentStatus;
            if (!backgroundWorker.IsBusy)
                IsCompleted = true;
            while (errors.Count > 0)
            {
                string curerr;
                errors.TryDequeue(out curerr);
                textBox1.Text += curerr + "\r\n\r\n";
            }

        }

        private void BackgroundWorker_DoWork(object sender, DoWorkEventArgs e)
        {
            currentStatus = "Formatting Drive " + DriveLetter + ":\\";
            if (!quickFormat)
                currentStatus += "\n (This process can take up to 30 minutes)";
            FormatDrive_CommandLine(ref currentStatus, DriveLetter, "BabbleVac", "FAT32", quickFormat);
            SoundFolder.ProcessAllSoundFolders(DriveLetter, ref currentStatus, errors);
            Thread.Sleep(1000);
            currentStatus = "Finished! Remove the SD Card safely from your pc.";
        }
    }
}
