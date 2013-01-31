namespace MultiIconTester
{
    partial class FormIconBrowser
    {
        /// <summary>
        /// Required designer variable.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// Clean up any resources being used.
        /// </summary>
        /// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Windows Form Designer generated code

        /// <summary>
        /// Required method for Designer support - do not modify
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(FormIconBrowser));
            this.btnBrowse = new System.Windows.Forms.Button();
            this.dlgBrowse = new System.Windows.Forms.FolderBrowserDialog();
            this.lblIcons = new System.Windows.Forms.Label();
            this.lbxIcons = new System.Windows.Forms.ListBox();
            this.lblImages = new System.Windows.Forms.Label();
            this.lbxImages = new System.Windows.Forms.ListBox();
            this.lblXORImage = new System.Windows.Forms.Label();
            this.lblAndImage = new System.Windows.Forms.Label();
            this.pbxIcon = new System.Windows.Forms.PictureBox();
            this.gbxFiles = new System.Windows.Forms.GroupBox();
            this.textBox1 = new System.Windows.Forms.TextBox();
            this.label1 = new System.Windows.Forms.Label();
            this.lblWidth = new System.Windows.Forms.Label();
            this.lblHeight = new System.Windows.Forms.Label();
            this.lblColorDepth = new System.Windows.Forms.Label();
            this.lblCompression = new System.Windows.Forms.Label();
            this.gbxImageInfo = new System.Windows.Forms.GroupBox();
            this.lblCompressionValue = new System.Windows.Forms.Label();
            this.lblHeightValue = new System.Windows.Forms.Label();
            this.lblColorDepthValue = new System.Windows.Forms.Label();
            this.lblWidthValue = new System.Windows.Forms.Label();
            this.btnExport = new System.Windows.Forms.Button();
            this.dlgSave = new System.Windows.Forms.SaveFileDialog();
            this.linkLabel1 = new System.Windows.Forms.LinkLabel();
            this.label2 = new System.Windows.Forms.Label();
            this.button1 = new System.Windows.Forms.Button();
            this.openFileDialog1 = new System.Windows.Forms.OpenFileDialog();
            this.label3 = new System.Windows.Forms.Label();
            this.textBox2 = new System.Windows.Forms.TextBox();
            this.button2 = new System.Windows.Forms.Button();
            this.label4 = new System.Windows.Forms.Label();
            this.radioButton1 = new System.Windows.Forms.RadioButton();
            this.radioButton2 = new System.Windows.Forms.RadioButton();
            this.numericUpDown1 = new System.Windows.Forms.NumericUpDown();
            this.numericUpDown2 = new System.Windows.Forms.NumericUpDown();
            this.label5 = new System.Windows.Forms.Label();
            this.textBox3 = new System.Windows.Forms.TextBox();
            this.textBox4 = new System.Windows.Forms.TextBox();
            this.label6 = new System.Windows.Forms.Label();
            this.label7 = new System.Windows.Forms.Label();
            this.label8 = new System.Windows.Forms.Label();
            this.textBox5 = new System.Windows.Forms.TextBox();
            ((System.ComponentModel.ISupportInitialize)(this.pbxIcon)).BeginInit();
            this.gbxFiles.SuspendLayout();
            this.gbxImageInfo.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.numericUpDown1)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.numericUpDown2)).BeginInit();
            this.SuspendLayout();
            // 
            // btnBrowse
            // 
            this.btnBrowse.Location = new System.Drawing.Point(12, 159);
            this.btnBrowse.Name = "btnBrowse";
            this.btnBrowse.Size = new System.Drawing.Size(75, 23);
            this.btnBrowse.TabIndex = 3;
            this.btnBrowse.Text = "Browse";
            this.btnBrowse.UseVisualStyleBackColor = true;
            this.btnBrowse.Click += new System.EventHandler(this.btnBrowse_Click);
            // 
            // lblIcons
            // 
            this.lblIcons.AutoSize = true;
            this.lblIcons.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.lblIcons.Location = new System.Drawing.Point(184, 16);
            this.lblIcons.Name = "lblIcons";
            this.lblIcons.Size = new System.Drawing.Size(38, 13);
            this.lblIcons.TabIndex = 6;
            this.lblIcons.Text = "Icons";
            // 
            // lbxIcons
            // 
            this.lbxIcons.FormattingEnabled = true;
            this.lbxIcons.Location = new System.Drawing.Point(187, 32);
            this.lbxIcons.Name = "lbxIcons";
            this.lbxIcons.Size = new System.Drawing.Size(170, 121);
            this.lbxIcons.TabIndex = 5;
            this.lbxIcons.SelectedIndexChanged += new System.EventHandler(this.lbxIcons_SelectedIndexChanged);
            // 
            // lblImages
            // 
            this.lblImages.AutoSize = true;
            this.lblImages.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.lblImages.Location = new System.Drawing.Point(360, 16);
            this.lblImages.Name = "lblImages";
            this.lblImages.Size = new System.Drawing.Size(47, 13);
            this.lblImages.TabIndex = 8;
            this.lblImages.Text = "Images";
            // 
            // lbxImages
            // 
            this.lbxImages.FormattingEnabled = true;
            this.lbxImages.Location = new System.Drawing.Point(363, 32);
            this.lbxImages.Name = "lbxImages";
            this.lbxImages.Size = new System.Drawing.Size(188, 121);
            this.lbxImages.TabIndex = 7;
            this.lbxImages.SelectedIndexChanged += new System.EventHandler(this.lbxImages_SelectedIndexChanged);
            // 
            // lblXORImage
            // 
            this.lblXORImage.AutoSize = true;
            this.lblXORImage.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.lblXORImage.Location = new System.Drawing.Point(9, 205);
            this.lblXORImage.Name = "lblXORImage";
            this.lblXORImage.Size = new System.Drawing.Size(71, 13);
            this.lblXORImage.TabIndex = 10;
            this.lblXORImage.Text = "XOR Image";
            // 
            // lblAndImage
            // 
            this.lblAndImage.AutoSize = true;
            this.lblAndImage.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.lblAndImage.Location = new System.Drawing.Point(271, 205);
            this.lblAndImage.Name = "lblAndImage";
            this.lblAndImage.Size = new System.Drawing.Size(71, 13);
            this.lblAndImage.TabIndex = 12;
            this.lblAndImage.Text = "AND Image";
            // 
            // pbxIcon
            // 
            this.pbxIcon.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
            this.pbxIcon.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;
            this.pbxIcon.Location = new System.Drawing.Point(629, 12);
            this.pbxIcon.Name = "pbxIcon";
            this.pbxIcon.Size = new System.Drawing.Size(256, 256);
            this.pbxIcon.SizeMode = System.Windows.Forms.PictureBoxSizeMode.CenterImage;
            this.pbxIcon.TabIndex = 13;
            this.pbxIcon.TabStop = false;
            // 
            // gbxFiles
            // 
            this.gbxFiles.Controls.Add(this.textBox1);
            this.gbxFiles.Controls.Add(this.btnBrowse);
            this.gbxFiles.Controls.Add(this.lbxIcons);
            this.gbxFiles.Controls.Add(this.lblIcons);
            this.gbxFiles.Controls.Add(this.lbxImages);
            this.gbxFiles.Controls.Add(this.lblImages);
            this.gbxFiles.Location = new System.Drawing.Point(12, 29);
            this.gbxFiles.Name = "gbxFiles";
            this.gbxFiles.Size = new System.Drawing.Size(563, 190);
            this.gbxFiles.TabIndex = 15;
            this.gbxFiles.TabStop = false;
            // 
            // textBox1
            // 
            this.textBox1.AllowDrop = true;
            this.textBox1.Location = new System.Drawing.Point(6, 19);
            this.textBox1.Multiline = true;
            this.textBox1.Name = "textBox1";
            this.textBox1.Size = new System.Drawing.Size(172, 134);
            this.textBox1.TabIndex = 0;
            this.textBox1.Text = "Drag and drop an exe here, or use the Browse button below";
            this.textBox1.DragDrop += new System.Windows.Forms.DragEventHandler(this.textBox1_DragDrop);
            this.textBox1.DragEnter += new System.Windows.Forms.DragEventHandler(this.textBox1_DragEnter);
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, ((System.Drawing.FontStyle)((System.Drawing.FontStyle.Bold | System.Drawing.FontStyle.Underline))), System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.label1.Location = new System.Drawing.Point(6, 16);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(108, 13);
            this.label1.TabIndex = 16;
            this.label1.Text = "Image Information";
            // 
            // lblWidth
            // 
            this.lblWidth.AutoSize = true;
            this.lblWidth.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.lblWidth.Location = new System.Drawing.Point(6, 46);
            this.lblWidth.Name = "lblWidth";
            this.lblWidth.Size = new System.Drawing.Size(44, 13);
            this.lblWidth.TabIndex = 17;
            this.lblWidth.Text = "Width:";
            // 
            // lblHeight
            // 
            this.lblHeight.AutoSize = true;
            this.lblHeight.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.lblHeight.Location = new System.Drawing.Point(6, 68);
            this.lblHeight.Name = "lblHeight";
            this.lblHeight.Size = new System.Drawing.Size(48, 13);
            this.lblHeight.TabIndex = 18;
            this.lblHeight.Text = "Height:";
            // 
            // lblColorDepth
            // 
            this.lblColorDepth.AutoSize = true;
            this.lblColorDepth.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.lblColorDepth.Location = new System.Drawing.Point(6, 90);
            this.lblColorDepth.Name = "lblColorDepth";
            this.lblColorDepth.Size = new System.Drawing.Size(78, 13);
            this.lblColorDepth.TabIndex = 19;
            this.lblColorDepth.Text = "Color Depth:";
            // 
            // lblCompression
            // 
            this.lblCompression.AutoSize = true;
            this.lblCompression.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.lblCompression.Location = new System.Drawing.Point(6, 112);
            this.lblCompression.Name = "lblCompression";
            this.lblCompression.Size = new System.Drawing.Size(82, 13);
            this.lblCompression.TabIndex = 20;
            this.lblCompression.Text = "Compression:";
            // 
            // gbxImageInfo
            // 
            this.gbxImageInfo.Controls.Add(this.lblCompressionValue);
            this.gbxImageInfo.Controls.Add(this.lblHeightValue);
            this.gbxImageInfo.Controls.Add(this.lblColorDepthValue);
            this.gbxImageInfo.Controls.Add(this.lblWidthValue);
            this.gbxImageInfo.Controls.Add(this.label1);
            this.gbxImageInfo.Controls.Add(this.lblCompression);
            this.gbxImageInfo.Controls.Add(this.lblWidth);
            this.gbxImageInfo.Controls.Add(this.lblColorDepth);
            this.gbxImageInfo.Controls.Add(this.lblHeight);
            this.gbxImageInfo.Location = new System.Drawing.Point(15, 225);
            this.gbxImageInfo.Name = "gbxImageInfo";
            this.gbxImageInfo.Size = new System.Drawing.Size(211, 190);
            this.gbxImageInfo.TabIndex = 21;
            this.gbxImageInfo.TabStop = false;
            // 
            // lblCompressionValue
            // 
            this.lblCompressionValue.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.lblCompressionValue.Location = new System.Drawing.Point(88, 112);
            this.lblCompressionValue.Name = "lblCompressionValue";
            this.lblCompressionValue.Size = new System.Drawing.Size(105, 13);
            this.lblCompressionValue.TabIndex = 24;
            // 
            // lblHeightValue
            // 
            this.lblHeightValue.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.lblHeightValue.Location = new System.Drawing.Point(88, 68);
            this.lblHeightValue.Name = "lblHeightValue";
            this.lblHeightValue.Size = new System.Drawing.Size(105, 13);
            this.lblHeightValue.TabIndex = 23;
            // 
            // lblColorDepthValue
            // 
            this.lblColorDepthValue.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.lblColorDepthValue.Location = new System.Drawing.Point(88, 90);
            this.lblColorDepthValue.Name = "lblColorDepthValue";
            this.lblColorDepthValue.Size = new System.Drawing.Size(121, 13);
            this.lblColorDepthValue.TabIndex = 22;
            // 
            // lblWidthValue
            // 
            this.lblWidthValue.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.lblWidthValue.Location = new System.Drawing.Point(88, 46);
            this.lblWidthValue.Name = "lblWidthValue";
            this.lblWidthValue.Size = new System.Drawing.Size(105, 13);
            this.lblWidthValue.TabIndex = 21;
            // 
            // btnExport
            // 
            this.btnExport.Location = new System.Drawing.Point(232, 231);
            this.btnExport.Name = "btnExport";
            this.btnExport.Size = new System.Drawing.Size(110, 32);
            this.btnExport.TabIndex = 22;
            this.btnExport.Text = "Export image";
            this.btnExport.UseVisualStyleBackColor = true;
            this.btnExport.Click += new System.EventHandler(this.btnExport_Click);
            // 
            // dlgSave
            // 
            this.dlgSave.DefaultExt = "icl";
            this.dlgSave.Filter = "Windows Icon File (*.ico)|*.ico|Icon Library File (*.icl)|*.icl|DLL Library File " +
                "(*.dll)|*.dll|PNG Image File (*.png)|*.png";
            this.dlgSave.SupportMultiDottedExtensions = true;
            this.dlgSave.Title = "Export MultiIcon to";
            // 
            // linkLabel1
            // 
            this.linkLabel1.Location = new System.Drawing.Point(65, 4);
            this.linkLabel1.Name = "linkLabel1";
            this.linkLabel1.Size = new System.Drawing.Size(260, 23);
            this.linkLabel1.TabIndex = 27;
            this.linkLabel1.TabStop = true;
            this.linkLabel1.Text = "http://www.codeproject.com/KB/cs/IconLib.aspx";
            this.linkLabel1.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            // 
            // label2
            // 
            this.label2.AutoSize = true;
            this.label2.Location = new System.Drawing.Point(12, 9);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(55, 13);
            this.label2.TabIndex = 28;
            this.label2.Text = "Based on:";
            // 
            // button1
            // 
            this.button1.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
            this.button1.Location = new System.Drawing.Point(653, 570);
            this.button1.Name = "button1";
            this.button1.Size = new System.Drawing.Size(233, 66);
            this.button1.TabIndex = 29;
            this.button1.Text = "Create AppWhirr app";
            this.button1.UseVisualStyleBackColor = true;
            this.button1.Click += new System.EventHandler(this.button1_Click_1);
            // 
            // openFileDialog1
            // 
            this.openFileDialog1.FileName = "openFileDialog1";
            this.openFileDialog1.Filter = "exe|*.exe";
            // 
            // label3
            // 
            this.label3.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
            this.label3.AutoSize = true;
            this.label3.Location = new System.Drawing.Point(610, 281);
            this.label3.Name = "label3";
            this.label3.Size = new System.Drawing.Size(275, 13);
            this.label3.TabIndex = 30;
            this.label3.Text = "Specify the folder of the app, which will be AppWhirr-ified";
            // 
            // textBox2
            // 
            this.textBox2.AllowDrop = true;
            this.textBox2.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
            this.textBox2.Location = new System.Drawing.Point(613, 303);
            this.textBox2.Multiline = true;
            this.textBox2.Name = "textBox2";
            this.textBox2.Size = new System.Drawing.Size(272, 78);
            this.textBox2.TabIndex = 31;
            this.textBox2.Text = "Drag and drop the folder here, or use the Browse button below";
            this.textBox2.DragDrop += new System.Windows.Forms.DragEventHandler(this.textBox2_DragDrop);
            this.textBox2.DragEnter += new System.Windows.Forms.DragEventHandler(this.textBox2_DragEnter);
            // 
            // button2
            // 
            this.button2.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
            this.button2.Location = new System.Drawing.Point(613, 388);
            this.button2.Name = "button2";
            this.button2.Size = new System.Drawing.Size(75, 23);
            this.button2.TabIndex = 32;
            this.button2.Text = "Browse";
            this.button2.UseVisualStyleBackColor = true;
            this.button2.Click += new System.EventHandler(this.button2_Click);
            // 
            // label4
            // 
            this.label4.AutoSize = true;
            this.label4.Location = new System.Drawing.Point(24, 431);
            this.label4.Name = "label4";
            this.label4.Size = new System.Drawing.Size(49, 13);
            this.label4.TabIndex = 33;
            this.label4.Text = "Icon size";
            // 
            // radioButton1
            // 
            this.radioButton1.AutoSize = true;
            this.radioButton1.Location = new System.Drawing.Point(24, 447);
            this.radioButton1.Name = "radioButton1";
            this.radioButton1.Size = new System.Drawing.Size(195, 17);
            this.radioButton1.TabIndex = 34;
            this.radioButton1.Text = "Use the original, selected icon\'s size";
            this.radioButton1.UseVisualStyleBackColor = true;
            // 
            // radioButton2
            // 
            this.radioButton2.AutoSize = true;
            this.radioButton2.Checked = true;
            this.radioButton2.Location = new System.Drawing.Point(24, 470);
            this.radioButton2.Name = "radioButton2";
            this.radioButton2.Size = new System.Drawing.Size(63, 17);
            this.radioButton2.TabIndex = 35;
            this.radioButton2.TabStop = true;
            this.radioButton2.Text = "Custom:";
            this.radioButton2.UseVisualStyleBackColor = true;
            // 
            // numericUpDown1
            // 
            this.numericUpDown1.Location = new System.Drawing.Point(93, 470);
            this.numericUpDown1.Maximum = new decimal(new int[] {
            1024,
            0,
            0,
            0});
            this.numericUpDown1.Minimum = new decimal(new int[] {
            2,
            0,
            0,
            0});
            this.numericUpDown1.Name = "numericUpDown1";
            this.numericUpDown1.Size = new System.Drawing.Size(63, 20);
            this.numericUpDown1.TabIndex = 36;
            this.numericUpDown1.Value = new decimal(new int[] {
            150,
            0,
            0,
            0});
            // 
            // numericUpDown2
            // 
            this.numericUpDown2.Location = new System.Drawing.Point(180, 470);
            this.numericUpDown2.Maximum = new decimal(new int[] {
            1024,
            0,
            0,
            0});
            this.numericUpDown2.Minimum = new decimal(new int[] {
            2,
            0,
            0,
            0});
            this.numericUpDown2.Name = "numericUpDown2";
            this.numericUpDown2.Size = new System.Drawing.Size(54, 20);
            this.numericUpDown2.TabIndex = 37;
            this.numericUpDown2.Value = new decimal(new int[] {
            150,
            0,
            0,
            0});
            // 
            // label5
            // 
            this.label5.AutoSize = true;
            this.label5.Location = new System.Drawing.Point(162, 472);
            this.label5.Name = "label5";
            this.label5.Size = new System.Drawing.Size(12, 13);
            this.label5.TabIndex = 38;
            this.label5.Text = "x";
            // 
            // textBox3
            // 
            this.textBox3.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
            this.textBox3.Location = new System.Drawing.Point(653, 544);
            this.textBox3.Name = "textBox3";
            this.textBox3.Size = new System.Drawing.Size(233, 20);
            this.textBox3.TabIndex = 39;
            this.textBox3.Text = "App name";
            // 
            // textBox4
            // 
            this.textBox4.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
            this.textBox4.Location = new System.Drawing.Point(520, 453);
            this.textBox4.Name = "textBox4";
            this.textBox4.Size = new System.Drawing.Size(365, 20);
            this.textBox4.TabIndex = 40;
            this.textBox4.Text = ".\\scripts\\easyAppsXmlDataCollector.py";
            // 
            // label6
            // 
            this.label6.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
            this.label6.AutoSize = true;
            this.label6.Location = new System.Drawing.Point(324, 456);
            this.label6.Name = "label6";
            this.label6.Size = new System.Drawing.Size(190, 13);
            this.label6.TabIndex = 41;
            this.label6.Text = "App InfoXml creator python script path:";
            // 
            // label7
            // 
            this.label7.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
            this.label7.AutoSize = true;
            this.label7.Location = new System.Drawing.Point(589, 547);
            this.label7.Name = "label7";
            this.label7.Size = new System.Drawing.Size(55, 13);
            this.label7.TabIndex = 42;
            this.label7.Text = "App name";
            // 
            // label8
            // 
            this.label8.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
            this.label8.AutoSize = true;
            this.label8.Location = new System.Drawing.Point(282, 483);
            this.label8.Name = "label8";
            this.label8.Size = new System.Drawing.Size(232, 13);
            this.label8.TabIndex = 43;
            this.label8.Text = "App Info Xml to Info Sqlite converter script path:";
            // 
            // textBox5
            // 
            this.textBox5.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
            this.textBox5.Location = new System.Drawing.Point(520, 480);
            this.textBox5.Name = "textBox5";
            this.textBox5.Size = new System.Drawing.Size(365, 20);
            this.textBox5.TabIndex = 44;
            this.textBox5.Text = ".\\scripts\\appInfoXmlToSqliteConverter.py";
            // 
            // FormIconBrowser
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(898, 649);
            this.Controls.Add(this.textBox5);
            this.Controls.Add(this.label8);
            this.Controls.Add(this.label7);
            this.Controls.Add(this.label6);
            this.Controls.Add(this.textBox4);
            this.Controls.Add(this.textBox3);
            this.Controls.Add(this.label5);
            this.Controls.Add(this.numericUpDown2);
            this.Controls.Add(this.numericUpDown1);
            this.Controls.Add(this.radioButton2);
            this.Controls.Add(this.radioButton1);
            this.Controls.Add(this.label4);
            this.Controls.Add(this.button2);
            this.Controls.Add(this.textBox2);
            this.Controls.Add(this.label3);
            this.Controls.Add(this.button1);
            this.Controls.Add(this.label2);
            this.Controls.Add(this.linkLabel1);
            this.Controls.Add(this.btnExport);
            this.Controls.Add(this.gbxImageInfo);
            this.Controls.Add(this.gbxFiles);
            this.Controls.Add(this.pbxIcon);
            this.Controls.Add(this.lblAndImage);
            this.Controls.Add(this.lblXORImage);
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedSingle;
            this.Icon = ((System.Drawing.Icon)(resources.GetObject("$this.Icon")));
            this.Name = "FormIconBrowser";
            this.StartPosition = System.Windows.Forms.FormStartPosition.CenterScreen;
            this.Text = "AppWhirr-izator [Based on: Icon Browser 0.73]";
            this.Load += new System.EventHandler(this.FormIconBrowser_Load);
            ((System.ComponentModel.ISupportInitialize)(this.pbxIcon)).EndInit();
            this.gbxFiles.ResumeLayout(false);
            this.gbxFiles.PerformLayout();
            this.gbxImageInfo.ResumeLayout(false);
            this.gbxImageInfo.PerformLayout();
            ((System.ComponentModel.ISupportInitialize)(this.numericUpDown1)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.numericUpDown2)).EndInit();
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.Button btnBrowse;
        private System.Windows.Forms.FolderBrowserDialog dlgBrowse;
        private System.Windows.Forms.Label lblIcons;
        private System.Windows.Forms.ListBox lbxIcons;
        private System.Windows.Forms.Label lblImages;
        private System.Windows.Forms.ListBox lbxImages;
        private System.Windows.Forms.Label lblXORImage;
        private System.Windows.Forms.Label lblAndImage;
        private System.Windows.Forms.PictureBox pbxIcon;
        private System.Windows.Forms.GroupBox gbxFiles;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.Label lblWidth;
        private System.Windows.Forms.Label lblHeight;
        private System.Windows.Forms.Label lblColorDepth;
        private System.Windows.Forms.Label lblCompression;
        private System.Windows.Forms.GroupBox gbxImageInfo;
        private System.Windows.Forms.Label lblCompressionValue;
        private System.Windows.Forms.Label lblHeightValue;
        private System.Windows.Forms.Label lblColorDepthValue;
        private System.Windows.Forms.Label lblWidthValue;
        private System.Windows.Forms.Button btnExport;
        private System.Windows.Forms.SaveFileDialog dlgSave;
        private System.Windows.Forms.LinkLabel linkLabel1;
        private System.Windows.Forms.Label label2;
        private System.Windows.Forms.Button button1;
        private System.Windows.Forms.TextBox textBox1;
        private System.Windows.Forms.OpenFileDialog openFileDialog1;
        private System.Windows.Forms.Label label3;
        private System.Windows.Forms.TextBox textBox2;
        private System.Windows.Forms.Button button2;
        private System.Windows.Forms.Label label4;
        private System.Windows.Forms.RadioButton radioButton1;
        private System.Windows.Forms.RadioButton radioButton2;
        private System.Windows.Forms.NumericUpDown numericUpDown1;
        private System.Windows.Forms.NumericUpDown numericUpDown2;
        private System.Windows.Forms.Label label5;
        private System.Windows.Forms.TextBox textBox3;
        private System.Windows.Forms.TextBox textBox4;
        private System.Windows.Forms.Label label6;
        private System.Windows.Forms.Label label7;
        private System.Windows.Forms.Label label8;
        private System.Windows.Forms.TextBox textBox5;
    }
}

