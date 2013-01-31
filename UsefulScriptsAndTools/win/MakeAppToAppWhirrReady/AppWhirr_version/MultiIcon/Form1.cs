using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using System.Runtime.InteropServices;
using System.IO;
using System.Drawing.Imaging;
using System.Drawing.IconLib;
using System.Drawing.IconLib.ColorProcessing;
using System.Diagnostics;

namespace MultiIconTester
{
    public partial class FormIconBrowser : Form
    {
        #region Variables Declaration
        private string      mFolder     = string.Empty;
        private MultiIcon   mMultiIcon  = new MultiIcon();
        #endregion

        #region Constructors
        public FormIconBrowser()
        {
            InitializeComponent();
            dlgSave.InitialDirectory = Path.GetDirectoryName(Application.ExecutablePath);
        }
        #endregion

        private string currentSourceAppExeFilePath = "";
        private string currentSourceAppFolder = "";

        #region Events
        private void FormIconBrowser_Load(object sender, EventArgs e)
        {
            try
            {
                //PopulateFiles(Path.GetDirectoryName(Application.ExecutablePath));
            }
            catch(Exception ex)
            {
                MessageBox.Show(this, ex.Message, "Error", MessageBoxButtons.OK, MessageBoxIcon.Error);
            }
        }

        private void btnBrowse_Click(object sender, EventArgs e)
        {
            /*dlgBrowse.SelectedPath = mFolder;
            dlgBrowse.ShowNewFolderButton = false;
            dlgBrowse.ShowDialog(this);

            if (dlgBrowse.SelectedPath == null || dlgBrowse.SelectedPath == string.Empty)
                return;

            PopulateFiles(dlgBrowse.SelectedPath);*/

            DialogResult dres = openFileDialog1.ShowDialog();
            if (dres == DialogResult.OK)
            {
                SetThisExeAsCurrent(openFileDialog1.FileName);
            }
        }

        void PresentIconsFromFile(string filePath)
        {
            try
            {
                lbxIcons.Items.Clear();
                lbxImages.Items.Clear();
                //pbxXORImage.Image = null;
                //pbxANDImage.Image = null;
                pbxIcon.Image = null;
                lblWidthValue.Text = null;
                lblHeightValue.Text = null;
                lblColorDepthValue.Text = null;
                lblCompressionValue.Text = null;
                mMultiIcon.SelectedIndex = -1;
                mMultiIcon.Load(filePath);

                foreach (SingleIcon singleIcon in mMultiIcon)
                    lbxIcons.Items.Add(singleIcon.Name);
            }
            catch (Exception ex)
            {
                MessageBox.Show(this, ex.Message, "Error", MessageBoxButtons.OK, MessageBoxIcon.Error);
            }
        }

        /*
        private void lbxFiles_SelectedIndexChanged(object sender, EventArgs e)
        {
            try
            {
                if (lbxFiles.SelectedIndex == -1)
                    return;

                lbxIcons.Items.Clear();
                lbxImages.Items.Clear();
                pbxXORImage.Image       = null;
                pbxANDImage.Image       = null;
                pbxIcon.Image           = null;
                lblWidthValue.Text      = null;
                lblHeightValue.Text     = null;
                lblColorDepthValue.Text = null;
                lblCompressionValue.Text= null;
                mMultiIcon.SelectedIndex= -1;
                mMultiIcon.Load(Path.Combine(mFolder, (string) lbxFiles.SelectedItem));

                foreach(SingleIcon singleIcon in mMultiIcon)
                    lbxIcons.Items.Add(singleIcon.Name);
            }
            catch(Exception ex)
            {
                MessageBox.Show(this, ex.Message, "Error", MessageBoxButtons.OK, MessageBoxIcon.Error);
            }
        }*/

        private void lbxIcons_SelectedIndexChanged(object sender, EventArgs e)
        {
            try
            {
                if (lbxIcons.SelectedIndex == -1)
                    return;

                lbxImages.Items.Clear();
                //pbxXORImage.Image       = null;
                //pbxANDImage.Image       = null;
                pbxIcon.Image           = null;
                lblWidthValue.Text      = null;
                lblHeightValue.Text     = null;
                lblColorDepthValue.Text = null;
                lblCompressionValue.Text= null;
                mMultiIcon.SelectedIndex= lbxIcons.SelectedIndex;
                foreach(IconImage iconImage in mMultiIcon[lbxIcons.SelectedIndex])
                {
                    lbxImages.Items.Add(iconImage.Size.Width + "x" + iconImage.Size.Height + " " + GetFriendlyBitDepth(iconImage.PixelFormat));
                }
            }
            catch(Exception ex)
            {
                MessageBox.Show(this, ex.Message, "Error", MessageBoxButtons.OK, MessageBoxIcon.Error);
            }
        }

        private void lbxImages_SelectedIndexChanged(object sender, EventArgs e)
        {
            try
            {
                if (lbxImages.SelectedIndex == -1)
                    return;

                IconImage iconImage     = mMultiIcon[lbxIcons.SelectedIndex][lbxImages.SelectedIndex];
                //pbxXORImage.Image       = iconImage.Image;
                //pbxANDImage.Image       = iconImage.Mask;
                pbxIcon.Image           = iconImage.Icon.ToBitmap();
                lblWidthValue.Text      = iconImage.Size.Width.ToString();
                lblHeightValue.Text     = iconImage.Size.Height.ToString();
                lblColorDepthValue.Text = iconImage.PixelFormat.ToString();
                lblCompressionValue.Text= iconImage.IconImageFormat.ToString();
            }
            catch(Exception ex)
            {
                MessageBox.Show(this, ex.Message, "Error", MessageBoxButtons.OK, MessageBoxIcon.Error);
            }
        }

        private void btnExport_Click(object sender, EventArgs e)
        {
            dlgSave.DefaultExt = "icl";
            dlgSave.Filter = "PNG Image File (*.png)|*.png|Windows Icon File (*.ico)|*.ico|Icon Library File (*.icl)|*.icl|DLL Library File " +
                "(*.dll)|*.dll|BMP Windows File (*.bmp)|*.bmp";
            if (dlgSave.ShowDialog(this) == DialogResult.OK)
            {
                if (dlgSave.FileName.ToLower().EndsWith(".ico") && mMultiIcon.SelectedIndex != -1)
                {
                    mMultiIcon.Save(dlgSave.FileName, MultiIconFormat.ICO);
                }
                else if (dlgSave.FileName.ToLower().EndsWith(".png") && mMultiIcon.SelectedIndex != -1 && lbxImages.SelectedIndex != -1)
                {
                    Bitmap iconBmp = mMultiIcon[mMultiIcon.SelectedIndex][lbxImages.SelectedIndex].Transparent;
                    if (this.radioButton2.Checked)
                    {
                        // convert it to the specified size
                        iconBmp = BitmapFineResize(iconBmp,
                            new Size((int)numericUpDown1.Value, (int)numericUpDown2.Value));
                    }
                    iconBmp.Save(dlgSave.FileName, ImageFormat.Png);
                }
                else if (dlgSave.FileName.ToLower().EndsWith(".bmp") && mMultiIcon.SelectedIndex != -1 && lbxImages.SelectedIndex != -1)
                {
                    Bitmap iconBmp = mMultiIcon[mMultiIcon.SelectedIndex][lbxImages.SelectedIndex].Transparent;
                    if (this.radioButton2.Checked)
                    {
                        // convert it to the specified size
                        iconBmp = BitmapFineResize(iconBmp,
                            new Size((int)numericUpDown1.Value, (int)numericUpDown2.Value));
                    }
                    iconBmp.Save(dlgSave.FileName, ImageFormat.Bmp);
                }
                else if (dlgSave.FileName.ToLower().EndsWith(".icl"))
                {
                    mMultiIcon.Save(dlgSave.FileName, MultiIconFormat.ICL);
                }
                else if (dlgSave.FileName.ToLower().EndsWith(".dll"))
                {
                    mMultiIcon.Save(dlgSave.FileName, MultiIconFormat.DLL);
                }
            }
        }

        private void btnExportAND_Click(object sender, EventArgs e)
        {
            dlgSave.DefaultExt = "bmp";
            dlgSave.Filter = "Windows Bitmap File (*.bmp)|*.bmp";
            if (mMultiIcon.SelectedIndex == -1 || lbxImages.SelectedIndex == -1)
                return;

            if (dlgSave.ShowDialog(this) == DialogResult.OK)
                mMultiIcon[mMultiIcon.SelectedIndex][lbxImages.SelectedIndex].Mask.Save(dlgSave.FileName, ImageFormat.Bmp);
        }

        private void btnExportBMP_Click(object sender, EventArgs e)
        {
            dlgSave.DefaultExt = "bmp";
            dlgSave.Filter = "Windows Bitmap File (*.bmp)|*.bmp";
            if (mMultiIcon.SelectedIndex == -1 || lbxImages.SelectedIndex == -1)
                return;

            if (dlgSave.ShowDialog(this) == DialogResult.OK)
                mMultiIcon[mMultiIcon.SelectedIndex][lbxImages.SelectedIndex].Image.Save(dlgSave.FileName, ImageFormat.Bmp);
        }
        #endregion

        #region Methods
        /*
        private void PopulateFiles(string folder)
        {
            List<string> validFiles = new List<string>();
            mFolder = folder;

//            string[] files = Directory.GetFiles(folder, "*.ico, *.icl, *.dll, *.exe, *.ocx, *.cpl, *.src");
            string[] files = Directory.GetFiles(folder);

            foreach(string file in files)
            {
                switch(Path.GetExtension(file).ToLower())
                {
                    case ".ico":
                    case ".icl":
                    case ".dll":
                    case ".exe":
                    case ".ocx":
                    case ".cpl":
                    case ".src":
                        validFiles.Add(Path.GetFileName(file));
                        break;
                }
            }
            lbxFiles.Items.Clear();
            lbxFiles.Items.AddRange(validFiles.ToArray());
        }*/

        private string GetFriendlyBitDepth(PixelFormat pixelFormat)
        {
            switch(pixelFormat)
            {
                case PixelFormat.Format1bppIndexed:
                    return "1-bit B/W";
                case PixelFormat.Format24bppRgb:
                    return "24-bit True Colors";
                case PixelFormat.Format32bppArgb:
                case PixelFormat.Format32bppRgb:
                    return "32-bit Alpha Channel";
                case PixelFormat.Format8bppIndexed:
                    return "8-bit 256 Colors";
                case PixelFormat.Format4bppIndexed:
                    return "4-bit 16 Colors";
            }
            return "Unknown";
        }
        #endregion

        #region Override
        protected override void OnClosing(CancelEventArgs e)
        {
            /*Form2 form = new Form2();
            form.ShowDialog(this);*/
            base.OnClosing(e);
        }
        #endregion

        #region Test methods
        /*
        private void btnCreate_Click(object sender, EventArgs e)
        {
            MultiIcon multiIcon = new MultiIcon();
            multiIcon.Add("Icon 1").Load(Path.Combine(Path.GetDirectoryName(Application.ExecutablePath), "Icon1.ico"));
            multiIcon.Add("Icon 2").Load(Path.Combine(Path.GetDirectoryName(Application.ExecutablePath), "Icon2.ico"));
            multiIcon.Add("Icon 3").Load(Path.Combine(Path.GetDirectoryName(Application.ExecutablePath), "Icon3.ico"));
            multiIcon.Add("Icon 4").Load(Path.Combine(Path.GetDirectoryName(Application.ExecutablePath), "Icon4.ico"));
            multiIcon.Add("Icon 5").Load(Path.Combine(Path.GetDirectoryName(Application.ExecutablePath), "Icon5.ico"));
            multiIcon.Add("Icon 6").Load(Path.Combine(Path.GetDirectoryName(Application.ExecutablePath), "Icon6.ico"));
            multiIcon.Add("Icon 7").Load(Path.Combine(Path.GetDirectoryName(Application.ExecutablePath), "Icon7.ico"));
            multiIcon.Save(Path.Combine(Path.GetDirectoryName(Application.ExecutablePath),"library.icl"), MultiIconFormat.ICL);
        }
         * */
        #endregion

        #region Internal Testing
        private void button1_Click(object sender, EventArgs e)
        {
            //MultiIcon mIcon = new MultiIcon();
            //SingleIcon sIcon = mIcon.Add("Icon1");
            //sIcon.CreateFrom("c:\\Pampero.png", IconOutputFormat.FromWin95);
            //sIcon.CreateFrom("C:\\z3.png", IconOutputFormat.FromWin31);
            //sIcon[0].Transparent.Save("c:\\zzzz1.png", ImageFormat.Png);
            //sIcon[1].Transparent.Save("c:\\zzzz2.png", ImageFormat.Png);
            //sIcon[2].Transparent.Save("c:\\zzzz3.png", ImageFormat.Png);
            //sIcon[3].Transparent.Save("c:\\zzzz4.png", ImageFormat.Png);
            //sIcon[4].Transparent.Save("c:\\zzzz5.png", ImageFormat.Png);

            //sIcon.Save("c:\\zzzz1.ico");
            //Bitmap bmp1 = (Bitmap) Bitmap.FromFile("C:\\test3.png");
            //bmp1.Save("c:\\test31.png", ImageFormat.Png);
            //Bitmap bmp2 = new Bitmap(bmp1, 128, 138);
            //bmp2.Save("c:\\test32.png", ImageFormat.Png);

            //IColorQuantizer colorReduction = new EuclideanQuantizer(new OctreeQuantizer(), new FloydSteinbergDithering());
            ////Bitmap bmp = (Bitmap) pbxIcon.Image;
            //Bitmap bmp = (Bitmap) Bitmap.FromFile("c:\\MyKids.png");
            ////DateTime dt1 = DateTime.Now;
            //Bitmap newBmp = colorReduction.Convert(bmp, PixelFormat.Format8bppIndexed);
            //newBmp.Save("c:\\MyKids8.png", ImageFormat.Png);

            //newBmp = colorReduction.Convert(bmp, PixelFormat.Format4bppIndexed);
            //newBmp.Save("c:\\MyKids4.png", ImageFormat.Png);

            //newBmp = colorReduction.Convert(bmp, PixelFormat.Format1bppIndexed);
            //newBmp.Save("c:\\MyKids1.png", ImageFormat.Png);

            ////newBmp = fs.Convert(bmp);
            ////newBmp = fs.Convert(bmp);
            //DateTime dt2 = DateTime.Now;
            //Console.WriteLine("Convert:" + ((TimeSpan) dt2.Subtract(dt1)).TotalMilliseconds);
            //pbxIcon.Image = newBmp;
            //newBmp.Save("c:\\zzz.bmp", ImageFormat.Bmp);
        }
        #endregion

        private void button2_Click(object sender, EventArgs e)
        {
            DialogResult dres = dlgBrowse.ShowDialog();
            if (dres == DialogResult.OK)
            {
                textBox2.Text = dlgBrowse.SelectedPath;
                SetThisFolderAsCurrent(dlgBrowse.SelectedPath);
            }
        }

        private void textBox1_DragEnter(object sender, DragEventArgs e)
        {
            if (e.Data.GetDataPresent(DataFormats.FileDrop, false) == true)
            {
                // allow them to continue
                // (without this, the cursor stays a "NO" symbol
                e.Effect = DragDropEffects.All;
            }
        }

        private void textBox1_DragDrop(object sender, DragEventArgs e)
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
                if (File.Exists(thePath) && Path.GetExtension(thePath).ToLower() == ".exe")
                {
                    SetThisExeAsCurrent(thePath);
                }
                else
                {
                    MessageBox.Show("Invalid: not an exe");
                }
            }
            catch (Exception ex)
            {
                MessageBox.Show(ex.ToString());
            }

            //label1.Text = "Got it";
        }

        private void SetThisExeAsCurrent(string thePath)
        {
            currentSourceAppExeFilePath = thePath;
            textBox1.Text = currentSourceAppExeFilePath;
            PresentIconsFromFile(currentSourceAppExeFilePath);

            // select the first icon, if any
            if(lbxIcons.Items.Count > 0)
                lbxIcons.SelectedIndex = 0;
            if (lbxImages.Items.Count > 0)
                lbxImages.SelectedIndex = 0;

            // and use the exe's name as app-name
            textBox3.Text = Path.GetFileNameWithoutExtension(thePath);
        }

        private void textBox2_DragEnter(object sender, DragEventArgs e)
        {
            if (e.Data.GetDataPresent(DataFormats.FileDrop, false) == true)
            {
                // allow them to continue
                // (without this, the cursor stays a "NO" symbol
                e.Effect = DragDropEffects.All;
            }
        }

        private void textBox2_DragDrop(object sender, DragEventArgs e)
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
                if (Directory.Exists(thePath))
                {
                    SetThisFolderAsCurrent(thePath);
                }
                else
                {
                    MessageBox.Show("Invalid: not a folder");
                }
            }
            catch (Exception ex)
            {
                MessageBox.Show(ex.ToString());
            }
        }

        private void SetThisFolderAsCurrent(string thePath)
        {
            currentSourceAppFolder = thePath;
            textBox2.Text = currentSourceAppFolder;
        }

        public static Bitmap BitmapFineResize(Bitmap originalImageToResize, System.Drawing.Size size)
        {
            int sourceWidth = originalImageToResize.Width;
            int sourceHeight = originalImageToResize.Height;

            float nPercent = 0;
            float nPercentW = 0;
            float nPercentH = 0;

            nPercentW = ((float)size.Width / (float)sourceWidth);
            nPercentH = ((float)size.Height / (float)sourceHeight);

            if (nPercentH < nPercentW)
                nPercent = nPercentH;
            else
                nPercent = nPercentW;


            //
            // first draw of the original image resized to the desired size
            Bitmap firstRenderBitmap = new Bitmap(size.Width, size.Height);
            using (Graphics g = Graphics.FromImage(firstRenderBitmap))
            {
                g.CompositingQuality = System.Drawing.Drawing2D.CompositingQuality.HighQuality;
                g.InterpolationMode = System.Drawing.Drawing2D.InterpolationMode.HighQualityBicubic;
                g.SmoothingMode = System.Drawing.Drawing2D.SmoothingMode.AntiAlias;

                g.CompositingMode = System.Drawing.Drawing2D.CompositingMode.SourceCopy;
                g.DrawImage(originalImageToResize, 0, 0, size.Width, size.Height);
            }

#if true
            //
            // first, upscale to very large
            int veryBigImageScaleRatio = 8;
            int veryBigImageWidth = size.Width * veryBigImageScaleRatio;
            int veryBigImageHeight = size.Height * veryBigImageScaleRatio;

            Bitmap veryBigBitmap = new Bitmap(veryBigImageWidth, veryBigImageHeight);
            using (Graphics g = Graphics.FromImage(veryBigBitmap))
            {
                g.CompositingQuality = System.Drawing.Drawing2D.CompositingQuality.AssumeLinear;
                g.InterpolationMode = System.Drawing.Drawing2D.InterpolationMode.Low;
                g.SmoothingMode = System.Drawing.Drawing2D.SmoothingMode.None;
                g.PixelOffsetMode = System.Drawing.Drawing2D.PixelOffsetMode.None;

                g.DrawImage(originalImageToResize, 0, 0, veryBigImageWidth, veryBigImageHeight);
            }
#endif

            //
            // then downscale to the final size
            Bitmap finalBitmap = new Bitmap(size.Width, size.Height);
            using (Graphics g = Graphics.FromImage(finalBitmap))
            {
                g.CompositingQuality = System.Drawing.Drawing2D.CompositingQuality.HighQuality;
                g.InterpolationMode = System.Drawing.Drawing2D.InterpolationMode.HighQualityBicubic;
                g.SmoothingMode = System.Drawing.Drawing2D.SmoothingMode.AntiAlias;

                g.CompositingMode = System.Drawing.Drawing2D.CompositingMode.SourceOver;

#if false
                g.DrawImage(veryBigBitmap, 0, 0, size.Width, size.Height);
#endif
                g.DrawImage(firstRenderBitmap, 0, 0, size.Width, size.Height);


#if true
                /*float[][] ptsArray ={ 
                new float[] {1, 0, 0, 0, 0},
                new float[] {0, 1, 0, 0, 0},
                new float[] {0, 0, 1, 0, 0},
                new float[] {0, 0, 0, 0.5f, 0}, 
                new float[] {0, 0, 0, 0, 1}}; 
                ColorMatrix clrMatrix = new ColorMatrix(ptsArray);*/
                // simplified version of the above one
                ColorMatrix clrMatrix = new ColorMatrix();
                clrMatrix.Matrix33 = 0.5f;
                //                clrMatrix.Matrix33 = 1.0f;
                ImageAttributes imgAttributes = new ImageAttributes();
                imgAttributes.SetColorMatrix(clrMatrix,
                    ColorMatrixFlag.Default,
                    ColorAdjustType.Bitmap);
                g.DrawImage(veryBigBitmap,
                    new Rectangle(0, 0, size.Width, size.Height),
                    0, 0, veryBigBitmap.Width, veryBigBitmap.Height,
                    GraphicsUnit.Pixel, imgAttributes);
#endif
            }

            return finalBitmap;
        }


        private void button1_Click_1(object sender, EventArgs e)
        {
            if (currentSourceAppFolder == "")
            {
                MessageBox.Show("Specify the folder of the source app.");
                return;
            }

            if (currentSourceAppExeFilePath == "")
            {
                MessageBox.Show("Specify the exe of the source app.");
                return;
            }

            if (mMultiIcon.SelectedIndex == -1 || lbxImages.SelectedIndex == -1)
            {
                MessageBox.Show("Select an icon from the exe, which will be used by AppWhirr.");
                return;
            }

            DialogResult dres = dlgBrowse.ShowDialog();
            if (dres == DialogResult.OK)
            {
                try
                {
                    string appName = "testName";
                    if (!String.IsNullOrEmpty(textBox3.Text))
                    {
                        appName = textBox3.Text;
                    }

                    // pathes and folder names
                    string targetRootDir = dlgBrowse.SelectedPath;
                    string appSpecSubdirName = "app_spec";
                    string targetAppSpecDir = Path.Combine(targetRootDir, appSpecSubdirName);
                    string appwhirrSpecSubdirName = "appwhirr_spec";
                    string appwhirrSpecSubFolder = Path.Combine(targetRootDir, appwhirrSpecSubdirName);
                    string iconSubfolder = "icons";
                    string iconFolder = Path.Combine(appwhirrSpecSubFolder, iconSubfolder);

                    // test the existance of required directories and create them if needed
                    if (!Directory.Exists(targetRootDir))
                    {
                        // create it
                        Directory.CreateDirectory(targetRootDir);
                    }
                    if (!Directory.Exists(targetAppSpecDir))
                    {
                        // create it
                        Directory.CreateDirectory(targetAppSpecDir);
                    }
                    if (!Directory.Exists(appwhirrSpecSubFolder))
                    {
                        // create it
                        Directory.CreateDirectory(appwhirrSpecSubFolder);
                    }
                    if (!Directory.Exists(iconFolder))
                    {
                        // create it
                        Directory.CreateDirectory(iconFolder);
                    }

                    // copy the source - app-folder
                    if(Directory.Exists(currentSourceAppFolder))
                    {
                        CopyDirectory(currentSourceAppFolder, targetAppSpecDir, true);
                    } else {
                        MessageBox.Show("Source folder doesn't exists", "Something bad happened");
                        return;
                    }

                    // save the app-icon
                    string iconFilePath = Path.Combine(iconFolder, (Path.GetFileNameWithoutExtension(currentSourceAppExeFilePath) + ".png"));
                    Bitmap iconBmp = mMultiIcon[mMultiIcon.SelectedIndex][lbxImages.SelectedIndex].Transparent;
                    if (this.radioButton2.Checked)
                    {
                        // convert it to the specified size
                        iconBmp = BitmapFineResize(iconBmp,
                            new Size((int)numericUpDown1.Value, (int)numericUpDown2.Value));
                    }
                    iconBmp.Save(iconFilePath, ImageFormat.Png);

                    string relativeIconPath = iconFilePath.Replace(targetRootDir, "");
                    if (relativeIconPath[0] == '\\')
                    {
                        relativeIconPath = "." + relativeIconPath;
                    }
                    //MessageBox.Show(relativeIconPath);

                    //
                    string relativeExePath = currentSourceAppExeFilePath.Replace(currentSourceAppFolder, "");
                    if (relativeExePath[0] == '\\')
                    {
                        relativeExePath = "./" + appSpecSubdirName + "/" + relativeExePath.Substring(1); // remove the initial \ from the beginning of the relative-exe-path
                            // and replace it with a normal / required by the python script
                    }
                    else
                    {
                        MessageBox.Show("The exe is outside from the source dir. Invalid.");
                        return;
                    }
                    string outputInfoXmlFileName = "app.xml";

                    // copy the xml-creator script, then run it
                    if(!File.Exists(textBox4.Text))
                    {
                        MessageBox.Show("The appXml creator script cannot be found.");
                        return;
                    }


                    //
                    // run the info xml creator script

                    string appXmlCreatorPythonScriptPath = Path.GetFullPath(textBox4.Text);
                    //MessageBox.Show(appXmlCreatorPythonScriptPath);
                    if (!File.Exists(appXmlCreatorPythonScriptPath))
                    {
                        MessageBox.Show("The xml creator python script cannot be found.");
                        return;
                    }

                    string copiedAppXmlCreatorPythonScriptPath = Path.Combine(targetRootDir, Path.GetFileName(appXmlCreatorPythonScriptPath));
                    //MessageBox.Show(copiedAppXmlCreatorPythonScriptPath);
                    File.Copy(appXmlCreatorPythonScriptPath, copiedAppXmlCreatorPythonScriptPath, true);

                    string scriptWorkingDir = targetRootDir;
                    bool isFailed = false;
                    if (!RunPythonScript(Path.GetFileName(appXmlCreatorPythonScriptPath), scriptWorkingDir,
                        new List<string>() { 
                                "\"" + appName + "\"", // app name
                                "__default", // svg icon path
                                "\"" + relativeIconPath + "\"", // large icon path
                                "__default", // medium icon path
                                "__default", // small icon path
                                relativeExePath,         // (relative) app exe path
                                outputInfoXmlFileName   // output_info_xml_path
                        }))
                    {
                        isFailed = true;
                    }

                    // delete the script
                    File.Delete(copiedAppXmlCreatorPythonScriptPath);

                    if (isFailed)
                    {
                        MessageBox.Show("Failed to run the script: " + appXmlCreatorPythonScriptPath);
                        return;
                    }


                    //
                    // run the info Xml to info Sqlite converter script

                    string appInfoXmlToInfoSqliteConverterPythonScriptPath = Path.GetFullPath(textBox5.Text);
                    if (!File.Exists(appInfoXmlToInfoSqliteConverterPythonScriptPath))
                    {
                        MessageBox.Show("The info xml to info sqlite converter python script cannot be found.");
                        return;
                    }

                    string copiedScriptPath = Path.Combine(targetRootDir, Path.GetFileName(appInfoXmlToInfoSqliteConverterPythonScriptPath));
                    File.Copy(appInfoXmlToInfoSqliteConverterPythonScriptPath, copiedScriptPath, true);

                    scriptWorkingDir = targetRootDir;
                    isFailed = false;
                    if (!RunPythonScript(Path.GetFileName(appInfoXmlToInfoSqliteConverterPythonScriptPath), scriptWorkingDir,
                        new List<string>() { 
                                "--sourceXmlFile=\"" + outputInfoXmlFileName + "\"", 
                                "--targetSQLiteFile=\"appInfo.sqlite\"" }))
                    {
                        isFailed = true;
                    }

                    // delete the script
                    File.Delete(copiedScriptPath);
                    // also delete the info xml file
                    File.Delete(targetRootDir + "\\" + outputInfoXmlFileName);

                    if (isFailed)
                    {
                        MessageBox.Show("Failed to run the script: " + appInfoXmlToInfoSqliteConverterPythonScriptPath);
                        return;
                    }


                    //
                    // we're finished, final, result showing
                    MessageBox.Show("Finished. Great! The folder will be shown soon. We have a new app! Have a nice day :)");
                    // and show the created app-whirr root directory
                    LocateThisPath(targetRootDir);
                }
                catch (Exception ex)
                {
                    MessageBox.Show(ex.ToString(), "Some error happened");
                }
            }
        }

        private bool RunPythonScript(string scriptName, string scriptWorkingDir, List<string> arguments)
        {
            // set process' infos
            Process p = new Process();
            p.StartInfo.RedirectStandardOutput = true;
            p.StartInfo.RedirectStandardError = true;
            p.StartInfo.UseShellExecute = false;
            p.StartInfo.CreateNoWindow = true;
            //p.StartInfo.StandardOutputEncoding
            p.StartInfo.WorkingDirectory = scriptWorkingDir;

            p.StartInfo.FileName = "python";
            p.StartInfo.Arguments = scriptName;
            foreach (string arg in arguments)
            {
                p.StartInfo.Arguments += " " + arg;
            }

            p.Start();
            p.WaitForExit(10000);

            if (p.HasExited)
            {
                /*richTextBox_code.Text = p.StandardOutput.ReadToEnd();
                richTextBox_compile_errors.Text = p.StandardError.ReadToEnd();

                textBox_args.Text = p.StartInfo.Arguments;*/
                string stdOut = p.StandardOutput.ReadToEnd();
                string stdErr = p.StandardError.ReadToEnd();

                bool isSucceeded = true;
                if (!String.IsNullOrEmpty(stdErr))
                {
                    //MessageBox.Show(stdErr, "XmlCreatorScript - error", MessageBoxButtons.OK, MessageBoxIcon.Error);
                    StatusMessagePresenterForm f = new StatusMessagePresenterForm(stdErr);
                    f.ShowDialog();
                    isSucceeded = false;
                }

                if (!String.IsNullOrEmpty(stdOut))
                {
                    //MessageBox.Show(stdOut, "XmlCreatorScript - output", MessageBoxButtons.OK, MessageBoxIcon.Information);
                    StatusMessagePresenterForm f = new StatusMessagePresenterForm(stdOut);
                    f.ShowDialog();
                }

                if (!isSucceeded)
                {
                    return false;
                }
            }

            return true;
        }

        private void CopyDirectory(string source_dir_path, string target_dir_path, bool is_overwrite)
        {
            //Now Create all of the directories
            foreach (string dirPath in Directory.GetDirectories(source_dir_path, "*", SearchOption.AllDirectories))
                Directory.CreateDirectory(dirPath.Replace(source_dir_path, target_dir_path));

            //Copy all the files
            foreach (string newPath in Directory.GetFiles(source_dir_path, "*.*", SearchOption.AllDirectories))
                File.Copy(newPath, newPath.Replace(source_dir_path, target_dir_path), is_overwrite);
        }

        private void LocateThisPath(string in_path)
        {
            // is it a file? if it's a file then get it's directory
            if (File.Exists(in_path))
            {
                in_path = Path.GetDirectoryName(in_path);
            }

            //MessageBox.Show(directory);

            if (Directory.Exists(in_path))
            {
                System.Diagnostics.Process.Start(in_path);
            }
            else
            {
                MessageBox.Show("Couldn't show the location. Invalid path");
            }
        }
    }
}