using System;
using System.Collections.Generic;
using System.Drawing;
using System.Drawing.Imaging;
using System.IO;
using System.Runtime.InteropServices;

namespace cquanttest
{
    class Program
    {

        static int addTransparentColorToPalette(Bitmap bitmap)
        {
            HashSet<byte> colorsSet=new HashSet<byte>();
            ColorPalette pal = bitmap.Palette;
            for (int i = 255; i > 0; i--)
            {
                pal.Entries[i] = pal.Entries[i - 1];
            }
            pal.Entries[0] = Color.FromArgb(255, 0, 255);
            bitmap.Palette = pal;

            BitmapData data = bitmap.LockBits(new Rectangle(0, 0, bitmap.Width, bitmap.Height), ImageLockMode.ReadWrite, PixelFormat.Format8bppIndexed);

            for(int y=0;y<data.Height;y++)
            {
                byte[] line = new byte[data.Width];
                Marshal.Copy(data.Scan0+ y * data.Stride, line, 0, data.Width);
                for(int i=0;i<line.Length;i++)
                {
                    byte col = line[i];
                    if (!colorsSet.Contains(col))
                        colorsSet.Add(col);
                    line[i] = (byte)(col+1);
                }
                Marshal.Copy(line, 0, data.Scan0+y * data.Stride, line.Length);
            }

            bitmap.UnlockBits(data);
            return colorsSet.Count + 1;
        }

        static void SetColorCountInBitmapFile(string filePath, UInt32 count)
        {
            using(FileStream fs=new FileStream(filePath,FileMode.Open,FileAccess.Write))
            {
                fs.Seek(46, SeekOrigin.Begin);
                byte[] countData = BitConverter.GetBytes(count);
                fs.Write(countData, 0, countData.Length);
                fs.Write(countData, 0, countData.Length);
            }
        }


        static void Main(string[] args)
        {
            if (args.Length <= 1)
                return;

            int maxColors = 255;

            string sourcePath = args[0];
            string targetPath = args[1];
            var quantizer = new PnnQuant.PnnQuantizer();
            using (var bitmap = new Bitmap(sourcePath))
            {
                //try
                {
                    using (var dest = quantizer.QuantizeImage(bitmap,PixelFormat.Format8bppIndexed,maxColors, true))
                    {
                        int colorCount = addTransparentColorToPalette(dest);
                        dest.Save(targetPath, ImageFormat.Bmp);
                        SetColorCountInBitmapFile(targetPath, (uint)colorCount);
                        System.Console.WriteLine("Converted image: " + targetPath);
                    }
                }
                /*catch (Exception q)
                {
                    System.Console.WriteLine(q.StackTrace);
                }*/
            }
        }
    }
}
