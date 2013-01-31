using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using ApplicationIconGetterAndSaver.Properties;
using System.IO;
using System.Drawing.Imaging;
using System.Runtime.InteropServices;

namespace ApplicationIconGetterAndSaver
{
    public partial class Form1 : Form
    {
        Bitmap theFinalBmp = null;
        string currentTargetFilePath = null;

        public Form1()
        {
            InitializeComponent();

            //textBox1.Text = Settings.Default.SavePath;
        }

        private void Form1_FormClosing(object sender, FormClosingEventArgs e)
        {
            //Settings.Default.SavePath = textBox1.Text;

            Settings.Default.Save();
        }

        private void Form1_DragEnter(object sender, DragEventArgs e)
        {
            if (e.Data.GetDataPresent(DataFormats.FileDrop, false) == true)
            {
                // allow them to continue
                // (without this, the cursor stays a "NO" symbol
                e.Effect = DragDropEffects.All;
            }
        }

        private void Form1_DragDrop(object sender, DragEventArgs e)
        {
            // transfer the filenames to a string array
            // (yes, everything to the left of the "=" can be put in the 
            // foreach loop in place of "files", but this is easier to understand.)
            string[] pathes = (string[])e.Data.GetData(DataFormats.FileDrop);

            if (pathes.Length > 0)
            {
                
            }
            else
            {
                MessageBox.Show("Invalid");
                return;
            }

            /*if (!Directory.Exists(textBox1.Text))
            {
                MessageBox.Show("Invalid target directory");
            }*/

            try
            {
                string thePath = pathes[0];
                if (File.Exists(thePath))
                {
                    currentTargetFilePath = thePath;
                    RefreshTheIconImage();
                }
            } 
            catch(Exception ex)
            {
                MessageBox.Show(ex.ToString());
            }

            label1.Text = "Got it";
        }

        private void button1_Click(object sender, EventArgs e)
        {
            if (theFinalBmp != null && currentTargetFilePath != null)
            {
                DialogResult dres = saveFileDialog1.ShowDialog();
                if (dres == DialogResult.OK)
                {
                    theFinalBmp.Save(saveFileDialog1.FileName, ImageFormat.Png);
                    MessageBox.Show("Saved");
                }
            }
            else
            {
                MessageBox.Show("First drag and drop an exe");
            }
        }




        #region Solution 2's defines, structs, ...

        // Constants that we need in the function call
        private const int SHGFI_ICON = 0x100;
        private const int SHGFI_SMALLICON = 0x1;
        private const int SHGFI_LARGEICON = 0x0;

        public struct SHFILEINFO
            {
                // Handle to the icon representing the file
                public IntPtr hIcon;
                // Index of the icon within the image list
                public int iIcon;
                // Various attributes of the file
                public uint dwAttributes;
                // Path to the file
                [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 256)]
                public string szDisplayName;
                // File type
                [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 80)]
                public string szTypeName;
            };

        // The signature of SHGetFileInfo (located in Shell32.dll)
        [DllImport("Shell32.dll")]
        public static extern IntPtr SHGetFileInfo(string pszPath, uint dwFileAttributes, ref SHFILEINFO psfi, int cbFileInfo, uint uFlags);

        #endregion

        #region Solution 3's utility stuffs

        // Based on: http://www.codeproject.com/KB/cs/IconExtractor.aspx
        // And a hint from: http://www.codeproject.com/KB/cs/IconLib.aspx

        Bitmap ExtractVistaIcon(Icon icoIcon)
        {
            Bitmap bmpPngExtracted = null;
            try
            {
                byte[] srcBuf = null;
                using (System.IO.MemoryStream stream = new System.IO.MemoryStream())
                { icoIcon.Save(stream); srcBuf = stream.ToArray(); }
                const int SizeICONDIR = 6;
                const int SizeICONDIRENTRY = 16;
                int iCount = BitConverter.ToInt16(srcBuf, 4);
                for (int iIndex = 0; iIndex < iCount; iIndex++)
                {
                    int iWidth = srcBuf[SizeICONDIR + SizeICONDIRENTRY * iIndex];
                    int iHeight = srcBuf[SizeICONDIR + SizeICONDIRENTRY * iIndex + 1];
                    int iBitCount = BitConverter.ToInt16(srcBuf, SizeICONDIR + SizeICONDIRENTRY * iIndex + 6);
                    if (iWidth == 0 && iHeight == 0 && iBitCount == 32)
                    {
                        int iImageSize = BitConverter.ToInt32(srcBuf, SizeICONDIR + SizeICONDIRENTRY * iIndex + 8);
                        int iImageOffset = BitConverter.ToInt32(srcBuf, SizeICONDIR + SizeICONDIRENTRY * iIndex + 12);
                        System.IO.MemoryStream destStream = new System.IO.MemoryStream();
                        System.IO.BinaryWriter writer = new System.IO.BinaryWriter(destStream);
                        writer.Write(srcBuf, iImageOffset, iImageSize);
                        destStream.Seek(0, System.IO.SeekOrigin.Begin);
                        bmpPngExtracted = new Bitmap(destStream); // This is PNG! :)
                        break;
                    }
                }
            }
            catch { return null; }
            return bmpPngExtracted;
        }

        #endregion


        private void RefreshTheIconImage()
        {
            if (currentTargetFilePath == null)
            {
                return;
            }

            //string saveImageHere = textBox1.Text + "/" + Path.GetFileNameWithoutExtension(thePath) + ".png";
            //label1.Text = thePath;


#if false // solution 1
            //Icon icon = GetIconHelper.GetFileIcon(currentTargetFilePath, GetIconHelper.IconSize.Large, false);
            //Icon icon = System.Drawing.Icon.ExtractAssociatedIcon(currentTargetFilePath);
#endif

#if false // solution 2 - winapi

            // Will store a handle to the small icon
            IntPtr hImgSmall;
            // Will store a handle to the large icon
            IntPtr hImgLarge;

            SHFILEINFO shinfo = new SHFILEINFO();
                // Sore the icon in this myIcon object
                System.Drawing.Icon myIcon;

                // Get a handle to the small icon
                hImgSmall = SHGetFileInfo(currentTargetFilePath, 0, ref shinfo, Marshal.SizeOf(shinfo), SHGFI_ICON | SHGFI_SMALLICON);
                // Get the small icon from the handle
                myIcon = System.Drawing.Icon.FromHandle(shinfo.hIcon);
                // Display the small icon
//                picIconSmall.Image = myIcon.ToBitmap();

                // Get a handle to the large icon
                hImgLarge = SHGetFileInfo(currentTargetFilePath, 0, ref shinfo, Marshal.SizeOf(shinfo), SHGFI_ICON | SHGFI_LARGEICON);
                // Get the large icon from the handle
                myIcon = System.Drawing.Icon.FromHandle(shinfo.hIcon);
                // Display the large icon
//                picIconLarge.Image = myIcon.ToBitmap();

#endif 

#if true // solution 3
            Icon icoAppIcon = null;
            // Getting FILL icon set from EXE, and extracting 256x256 version for logo...
            using (TKageyu.Utils.IconExtractor IconEx = new TKageyu.Utils.IconExtractor(Application.ExecutablePath))
            {
                icoAppIcon = IconEx.GetIcon(0); // Because standard System.Drawing.Icon.ExtractAssociatedIcon() returns ONLY 32x32.
            }
#endif

            //Bitmap bmp = (Bitmap)icon.ToBitmap().Clone();
            Bitmap bmp = ExtractVistaIcon(icoAppIcon);
            theFinalBmp = GetIconHelper.ResizeToJumbo(bmp, new Size((int)numericUpDown1.Value, (int)numericUpDown2.Value), 0);
            //Icon saveableIcon = Icon.FromHandle(bmp.GetHicon());
            //FileStream fs = new FileStream(textBox1.Text + "/" + Path.GetFileNameWithoutExtension(thePath) + ".png", FileMode.Create);
            //saveableIcon.Save(fs);

            //fs.Close();

            pictureBox1.Image = theFinalBmp;
        }

        private void numericUpDown1_ValueChanged(object sender, EventArgs e)
        {
            RefreshTheIconImage();
        }

        private void numericUpDown2_ValueChanged(object sender, EventArgs e)
        {
            RefreshTheIconImage();
        }
    }
}
