using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Drawing;
using System.Data;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace BabbleVacSDClient.Controls
{
    public partial class StepBaseControl : UserControl
    {
        private bool isCompleted;
        public event EventHandler IsCompletedChanged;

        public bool IsCompleted
        {
            get => isCompleted;
            protected set
            {
                if (IsCompletedChanged != null)
                    IsCompletedChanged(this, new EventArgs());
                isCompleted = value;
            }
        }

        public StepBaseControl()
        {
            InitializeComponent();
        }
    }
}
