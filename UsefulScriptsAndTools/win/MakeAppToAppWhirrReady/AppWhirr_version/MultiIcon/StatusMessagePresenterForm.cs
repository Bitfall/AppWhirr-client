using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;

namespace MultiIconTester
{
    public partial class StatusMessagePresenterForm : Form
    {
        public StatusMessagePresenterForm(String messageToPresent)
        {
            InitializeComponent();

            this.richTextBox1.Text = messageToPresent;
        }
    }
}
