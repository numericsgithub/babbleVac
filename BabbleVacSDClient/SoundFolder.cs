using System;
using System.Collections.Concurrent;
using System.Collections.Generic;
using System.Diagnostics;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace BabbleVacSDClient
{
    public class SoundFolder
    {
        public delegate void UpdateFolder(SoundFolder sender);

        private FileSystemWatcher watcher;
        private readonly char FILE_PREFIX;

        public static readonly string PARENT_FOLDER = Environment.CurrentDirectory + "\\soundFolder";

        public string Name { get; private set; }
        public string Path => PARENT_FOLDER + "\\" + Name;
        public string[] Content => Directory.GetFiles(Path);
        public event UpdateFolder FolderUpdated;

        public SoundFolder(string name, char prefix)
        {
            this.Name = name;
            this.FILE_PREFIX = prefix;
            Directory.CreateDirectory(PARENT_FOLDER);
            Directory.CreateDirectory(Path);
            watcher = new FileSystemWatcher(Path);
            watcher.Changed += Watcher_Changed;
        }

        private void Watcher_Changed(object sender, FileSystemEventArgs e)
        {
            if (FolderUpdated != null)
                FolderUpdated(this);
        }

        private bool ProcessSoundFile(string filename, char driveLetter, int fileNumber, ConcurrentQueue<string> errors)
        {
            bool success = false;
            try
            {
                string outputFileName = FILE_PREFIX + fileNumber.ToString("000") + ".wav";
                outputFileName = driveLetter + ":\\" + outputFileName;
                //sox %%A --norm=-1 -e unsigned-integer -b 8 -r 62500 -c 1 -t raw "converted/%%~nxA"
                var psi = new ProcessStartInfo();
                psi.FileName = Environment.CurrentDirectory + "\\sox.exe";
                psi.CreateNoWindow = true; //if you want to hide the window
                psi.WorkingDirectory = Environment.CurrentDirectory;
                psi.Arguments = "\"" + filename + "\" --norm=-1 -e unsigned-integer -b 8 -r 62500 -c 1 -t raw " + outputFileName;
                psi.UseShellExecute = false;
                psi.CreateNoWindow = true;
                psi.RedirectStandardOutput = true;
                psi.RedirectStandardInput = true;
                var formatProcess = Process.Start(psi);
                var swStandardInput = formatProcess.StandardInput;
                swStandardInput.WriteLine();
                formatProcess.WaitForExit();
                if (File.Exists(outputFileName))
                    success = true;
                else errors.Enqueue("Error Processing soundfile " + filename + " was not saved or processed properly.");
            }
            catch (Exception ex) { errors.Enqueue("Error Processing soundfile " + filename + " " + ex.Message); }
            return success;
        }

        public void ProcessAllSoundfiles(char driveLetter, ref string status, ConcurrentQueue<string> errors)
        {
            int fileNumber = 0;
            string[] files = Directory.GetFiles(Path).Where(f => f.EndsWith(".wav") || f.EndsWith(".mp3")).ToArray();
            foreach (var soundfile in files)
            {
                status = "Processing " + soundfile;
                if (!ProcessSoundFile(soundfile, driveLetter, fileNumber++, errors))
                {
                    fileNumber--;
                }
            }
        }

        public static void ProcessAllSoundFolders(char driveLetter, ref string status, ConcurrentQueue<string> errors)
        {
            AfterStart.ProcessAllSoundfiles(driveLetter, ref status, errors);
            AfterEnd.ProcessAllSoundfiles(driveLetter, ref status, errors);
            OnCollision.ProcessAllSoundfiles(driveLetter, ref status, errors);
            WhileCleaning.ProcessAllSoundfiles(driveLetter, ref status, errors);
            LiftUp.ProcessAllSoundfiles(driveLetter, ref status, errors);
        }

        public static SoundFolder AfterStart = new SoundFolder("On cleaing start", 's');
        public static SoundFolder AfterEnd = new SoundFolder("On cleaing end", 'e');
        public static SoundFolder OnCollision = new SoundFolder("On collision", 'c');
        public static SoundFolder WhileCleaning = new SoundFolder("While cleaning", 'w');
        public static SoundFolder LiftUp = new SoundFolder("On lift", 'l');
    }
}
