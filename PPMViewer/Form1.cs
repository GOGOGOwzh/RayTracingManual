using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using System.IO;

namespace PPMViewer
{
    public partial class Form1 : Form
    {
        public Form1()
        {
            InitializeComponent();
        }

        private void Form1_Load(object sender, EventArgs e)
        {
            OpenFileDialog file = new OpenFileDialog();
            file.InitialDirectory = Environment.GetFolderPath(Environment.SpecialFolder.LocalizedResources);
            file.Filter = "(ppm文件)*.ppm|*.ppm";
            file.Title = "请选择图像";
            if (file.ShowDialog() == DialogResult.OK)
            {
                FileStream fs = new FileStream(file.FileName, FileMode.Open);
                StreamReader sr = new StreamReader(fs);
                string format = sr.ReadLine();
                format = format.ToLower();
                if (format != "p3")
                {
                    MessageBox.Show("目前只支持P3格式PPM！请输出P3格式PPM");
                    return;
                }
                string size = sr.ReadLine();
                string colorSize = sr.ReadLine();

                string[] strSize = size.Split(' ');
                int width = int.Parse(strSize[0]);
                int hight = int.Parse(strSize[1]);

                this.MinimumSize = new Size(width, hight);

                Bitmap bitmap = new Bitmap(width, hight);
                for (int i = 0; i < hight; i++)
                {
                    for (int j = 0; j < width; j++)
                    {
                        string colorLine = sr.ReadLine();
                        string[] pixels = colorLine.Split(' ');
                        bitmap.SetPixel(j, i, Color.FromArgb(int.Parse(pixels[0]), int.Parse(pixels[1]), int.Parse(pixels[2])));
                    }
                }
                this.BackgroundImage = bitmap;
                this.BackgroundImageLayout = ImageLayout.Stretch;
            }
        }
    }
}
