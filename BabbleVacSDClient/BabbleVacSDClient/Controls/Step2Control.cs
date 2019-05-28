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

namespace BabbleVacSDClient.Controls
{
    public partial class Step2Control : StepBaseControl
    {
        public bool QuickFormat => checkBox1.Checked;
        
        public Step2Control()
        {
            InitializeComponent();
            IsCompleted = true;
        }

        private void Button1_Click(object sender, EventArgs e)
        {
            Process.Start(SoundFolder.PARENT_FOLDER);
        }
    }
}
