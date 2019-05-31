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

namespace BabbleVacSDClient.Controls
{
    public partial class Step1Control : StepBaseControl
    {
        private List<DriveInfo> sdCards;
        public DriveInfo SelectedDrive => sdCards[cbxDrive.SelectedIndex];

        public Step1Control()
        {
            InitializeComponent();
        }

        private bool IsDifferent(List<DriveInfo> list)
        {
            if (sdCards == null && list == null)
                return false;
            if (sdCards == null || list == null)
                return true;
            if (sdCards.Count != list.Count)
                return true;
            for (int i = 0; i < list.Count; i++)
            {
                if (sdCards.FirstOrDefault(d => d.Name == list[i].Name) == null)
                    return true;
            }
            return false;
        }

        private void UpdateSDCards()
        {
            List<DriveInfo> driveInfos = DriveInfo.GetDrives()
                .Where(drive => drive.DriveType == DriveType.Removable).ToList();
            if (IsDifferent(driveInfos))
            {
                cbxDrive.Text = "";
                cbxDrive.Items.Clear();
                cbxDrive.Items.AddRange(driveInfos.ToArray());
                sdCards = driveInfos;
                if (sdCards == null || sdCards.Count == 0)
                    cbxDrive.Items.Add("<No SD Cards found>");
                cbxDrive.SelectedIndex = 0;
                if (sdCards == null || sdCards.Count == 0)
                    IsCompleted = false;
                else IsCompleted = true;
            }
        }

        private void CbxDrive_SelectedIndexChanged(object sender, EventArgs e)
        {
            IsCompleted = sdCards != null && sdCards.Count != 0 && cbxDrive.SelectedIndex != -1;
        }

        private void Timer_Tick(object sender, EventArgs e)
        {
            UpdateSDCards();
        }
    }
}
