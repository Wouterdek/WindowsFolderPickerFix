using System;
using System.Collections.Generic;
using System.IO;
using System.Text;
using Microsoft.Deployment.WindowsInstaller;
using Microsoft.Win32;

namespace RegistryModifier
{
    public class CustomActions
    {
        private static void InstallDLLHook(RegistryKey localMachine)
        {
            RegistryKey windowsKey = localMachine.OpenSubKey(@"SOFTWARE\Microsoft\Windows NT\CurrentVersion\Windows", true);
            object loadDlls = windowsKey.GetValue("LoadAppInit_DLLs");
            object appInitDlls = windowsKey.GetValue("AppInit_DLLs");

            //Set LoadAppInit_DLLs to 1
            if (loadDlls is int && (int)loadDlls == 0 && appInitDlls is string && !((string)appInitDlls).Trim().Equals(""))
            {
                windowsKey.SetValue("AppInit_DLLs_Backup", appInitDlls);
                windowsKey.SetValue("AppInit_DLLs", "");
            }
            windowsKey.SetValue("LoadAppInit_DLLs", 1, RegistryValueKind.DWord);

            //Set RequireSignedAppInit_DLLs to 0
            windowsKey.SetValue("RequireSignedAppInit_DLLs", 0, RegistryValueKind.DWord);

            //Set AppInit_DLLs
            string folder = @"C:\ProgramData\WindowsFolderPickerFix\";
            string newAppInitDlls = folder+"x86.dll "+ folder+"x64.dll";
            newAppInitDlls += appInitDlls is string ? " "+(string)appInitDlls : "";

            windowsKey.SetValue("AppInit_DLLs", newAppInitDlls);
        }

        private static void RemoveDLLHook(RegistryKey localMachine)
        {
            RegistryKey windowsKey = localMachine.OpenSubKey(@"SOFTWARE\Microsoft\Windows NT\CurrentVersion\Windows", true);
            object appInitDlls = windowsKey.GetValue("AppInit_DLLs");

            if (appInitDlls is string)
            {
                appInitDlls = ((string) appInitDlls).Replace(@"C:\ProgramData\WindowsFolderPickerFix\x86.dll", "");
                appInitDlls = ((string)appInitDlls).Replace(@"C:\ProgramData\WindowsFolderPickerFix\x64.dll", "");
                windowsKey.SetValue("AppInit_DLLs", appInitDlls);

                if (((string)appInitDlls).Trim().Equals(""))
                {
                    windowsKey.SetValue("LoadAppInit_DLLs", 0, RegistryValueKind.DWord);
                }
            }
        }

        [CustomAction]
        public static ActionResult InstallDLLHook(Session session)
        {
            session.Log("Begin InstallDLLHook");
            
            if (Environment.Is64BitOperatingSystem)
            {
                RegistryKey localMachine64 = RegistryKey.OpenBaseKey(Microsoft.Win32.RegistryHive.LocalMachine, RegistryView.Registry64);
                InstallDLLHook(localMachine64);
            }

            RegistryKey localMachine32 = RegistryKey.OpenBaseKey(Microsoft.Win32.RegistryHive.LocalMachine, RegistryView.Registry32);
            InstallDLLHook(localMachine32);

            return ActionResult.Success;
        }

        [CustomAction]
        public static ActionResult RemoveDLLHook(Session session)
        {
            session.Log("Begin RemoveDLLHook");

            if (Environment.Is64BitOperatingSystem)
            {
                RegistryKey localMachine64 = RegistryKey.OpenBaseKey(Microsoft.Win32.RegistryHive.LocalMachine, RegistryView.Registry64);
                RemoveDLLHook(localMachine64);
            }

            RegistryKey localMachine32 = RegistryKey.OpenBaseKey(Microsoft.Win32.RegistryHive.LocalMachine, RegistryView.Registry32);
            RemoveDLLHook(localMachine32);

            return ActionResult.Success;
        }
    }
}
