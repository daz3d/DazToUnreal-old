using Microsoft.Win32;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading.Tasks;
using System.Windows.Forms;
using System.IO;

namespace DazToUnrealSetup
{
    static class Program
    {
        /// <summary>
        /// The main entry point for the application.
        /// </summary>
        [STAThread]
        static void Main(string[] args)
        {
            try
            {
                using (RegistryKey key = Registry.ClassesRoot.OpenSubKey("dazfile\\shell\\open\\command"))
                {
                    if (key != null)
                    {
                        Object obj = key.GetValue("");
                        if (obj != null)
                        {
                            String value = obj.ToString();
                            String exePath = value.Split(new string[] { "\"" }, StringSplitOptions.None)[1];
                            String pluginDirectory = Path.Combine(Path.GetDirectoryName(exePath), "plugins", "DazToUnreal.dll");
                            File.Copy(Path.Combine(AppDomain.CurrentDomain.BaseDirectory, "DazToUnreal.dll"), pluginDirectory, true);
                            return;
                        }
                    }
                }
            }
            catch (Exception e)
            {
               
            }

            try
            {
                if (Directory.Exists(@"C:\Program Files\DAZ 3D\DAZStudio4\plugins"))
                {
                    String pluginDirectory = Path.Combine(@"C:\Program Files\DAZ 3D\DAZStudio4\plugins", "plugins", "DazToUnreal.dll");
                    File.Copy(Path.Combine(AppDomain.CurrentDomain.BaseDirectory, "DazToUnreal.dll"), pluginDirectory, true);
                    return;
                }
            }
            catch (Exception e)
            {

            }
            MessageBox.Show(String.Format("Please copy DazToUnreal.dll from\n{0}\n to the Daz Studio plugin folder.", AppDomain.CurrentDomain.BaseDirectory),
                "Install Failed",
                MessageBoxButtons.OK,
                MessageBoxIcon.Error);
        }
    }
}
