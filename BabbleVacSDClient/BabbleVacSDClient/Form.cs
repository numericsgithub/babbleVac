using BabbleVacSDClient.Controls;
using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace BabbleVacSDClient
{
    public partial class Form : System.Windows.Forms.Form
    {
        private int stepIndex = 1;

        private Step1Control step1Control; 
        private Step2Control step2Control; 
        private Step3Control step3Control;
        private StepBaseControl current;

        public Form()
        {
            InitializeComponent();
            UpdateStep();
        }

        private void UpdateStepEnable()
        {
            btnNext.Enabled = current.IsCompleted;
            btnPrevious.Enabled = stepIndex != 3;
        }

        private void UpdateStepBtns(string previous, string next)
        {
            btnNext.Text = next;
            btnPrevious.Text = previous;
            UpdateStepEnable();
        }

        private void UpdateStep()
        {
            switch (stepIndex)
            {
                case 0:
                    this.Close();
                    break;
                case 1:
                    if (step1Control == null)
                    {
                        step1Control = new Step1Control();
                        step1Control.IsCompletedChanged += Step1Control_IsCompletedChanged;
                    }
                    pContent.Controls.Clear();
                    pContent.Controls.Add(step1Control);
                    current = step1Control;
                    UpdateStepBtns("Exit", "Continue");
                    break;
                case 2:
                    if (step2Control == null)
                    {
                        step2Control = new Step2Control();
                        step2Control.IsCompletedChanged += Step2Control_IsCompletedChanged;
                    }
                    pContent.Controls.Clear();
                    pContent.Controls.Add(step2Control);
                    current = step2Control;
                    UpdateStepBtns("Previous", "Flash Data");
                    break;
                case 3:
                    if (step3Control == null)
                    {

                        string name;
                        try
                        {
                            name = step1Control.SelectedDrive.Name;
                            name = name.Replace(":\\", "");
                            if (name.Length != 1)
                                throw new Exception();
                        }
                        catch
                        {
                            MessageBox.Show("The selected drive is not valid!");
                            stepIndex = 1;
                            UpdateStep();
                            return;
                        }
                        step3Control = new Step3Control(name[0], step2Control.QuickFormat);
                        step3Control.IsCompletedChanged += Step3Control_IsCompletedChanged;
                    }
                    pContent.Controls.Clear();
                    pContent.Controls.Add(step3Control);
                    current = step3Control;
                    btnPrevious.Visible = false;
                    UseWaitCursor = true;
                    UpdateStepBtns("Previous", "Exit");
                    step3Control.Start();
                    break;
                case 4:
                    this.Close();
                    break;
                default:
                    break;
            }
        }

        private void Step3Control_IsCompletedChanged(object sender, EventArgs e)
        {
            if (step3Control.IsCompleted)
            {
                UseWaitCursor = false;
                btnNext.Enabled = true;
            }
        }

        private void Step2Control_IsCompletedChanged(object sender, EventArgs e)
        {
            UpdateStepEnable();
        }

        private void Step1Control_IsCompletedChanged(object sender, EventArgs e)
        {
            UpdateStepEnable();
            if (!step1Control.IsCompleted && stepIndex == 2)
            {
                MessageBox.Show("The SD Card has been removed!");
                stepIndex--;
                UpdateStep();
            }
        }

        private void BtnNext_Click(object sender, EventArgs e)
        {
            stepIndex++;
            UpdateStep();
        }

        private void BtnPrevious_Click(object sender, EventArgs e)
        {
            stepIndex--;
            UpdateStep();
        }
    }
}
