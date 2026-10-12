param([Parameter(Mandatory = $true)][string]$Fixture)
$ErrorActionPreference = 'Stop'
Add-Type -AssemblyName System.Drawing
Add-Type -AssemblyName System.Windows.Forms
Add-Type @'
using System;
using System.Runtime.InteropServices;
using System.Text;
public static class DailyGui {
    [DllImport("user32.dll")]
    public static extern IntPtr GetDlgItem(IntPtr w, int id);
    [DllImport("user32.dll", CharSet = CharSet.Unicode)]
    public static extern IntPtr SendMessage(IntPtr w, uint m, IntPtr wp, string text);
    [DllImport("user32.dll")]
    public static extern IntPtr SendMessage(IntPtr w, uint m, IntPtr wp, IntPtr lp);
    [DllImport("user32.dll")]
    public static extern bool PostMessage(IntPtr w, uint m, IntPtr wp, IntPtr lp);
    [DllImport("user32.dll", CharSet = CharSet.Unicode)]
    public static extern int GetWindowText(IntPtr w, StringBuilder text, int count);
    [DllImport("user32.dll", CharSet = CharSet.Unicode)]
    public static extern IntPtr SendMessage(IntPtr w, uint m, IntPtr wp, StringBuilder text);
    [DllImport("user32.dll")]
    public static extern bool GetClientRect(IntPtr w, out Rect r);
    [DllImport("user32.dll")]
    public static extern bool GetWindowRect(IntPtr w, out Rect r);
    [DllImport("user32.dll")]
    public static extern bool PrintWindow(IntPtr w, IntPtr dc, uint flags);
    [DllImport("user32.dll")]
    public static extern bool ShowWindow(IntPtr w, int how);
    [DllImport("user32.dll")]
    public static extern bool IsWindow(IntPtr w);
    [DllImport("user32.dll")]
    public static extern int GetWindowLong(IntPtr w, int index);
    [DllImport("user32.dll", CharSet = CharSet.Unicode)]
    public static extern int GetWindowTextLength(IntPtr w);
    public delegate bool EnumFn(IntPtr w, IntPtr p);
    [DllImport("user32.dll")]
    public static extern bool EnumWindows(EnumFn fn, IntPtr p);
    [DllImport("user32.dll")]
    public static extern uint GetWindowThreadProcessId(IntPtr w, out uint pid);
    public static IntPtr Find(int pid) {
        IntPtr found = IntPtr.Zero;
        EnumWindows((w, p) => {uint owner;GetWindowThreadProcessId(w,out owner);if (owner==pid && GetDlgItem(w,100)!=IntPtr.Zero){found=w;return false;}return true; }, IntPtr.Zero);
        return found;
    }
    [DllImport("user32.dll", CharSet = CharSet.Unicode)]
    public static extern int GetClassName(IntPtr window, StringBuilder text, int size);
    [DllImport("user32.dll")]
    public static extern bool EnumChildWindows(IntPtr window, EnumFn fn, IntPtr data);
    public static IntPtr Dialog(int pid) {
        IntPtr found = IntPtr.Zero;
        EnumWindows((w, p) => {uint owner;GetWindowThreadProcessId(w,out owner);var name=new StringBuilder(64);GetClassName(w,name,64);if (owner==pid&&name.ToString()=="#32770"){found=w;return false;}return true; }, IntPtr.Zero);
        return found;
    }
    public static string ChildrenText(IntPtr window) {
        var all = new StringBuilder();
        EnumChildWindows(window, (w, p) => {var text=new StringBuilder(2048);GetWindowText(w,text,2048);all.AppendLine(text.ToString());return true; }, IntPtr.Zero);
        return all.ToString();
    }
    [DllImport("user32.dll")]
    public static extern bool OpenClipboard(IntPtr owner);
    [DllImport("user32.dll")]
    public static extern bool CloseClipboard();
    public static void SetText(IntPtr window, string text) {
        SendMessage(window, 0x000c, IntPtr.Zero, text);
    }
    [StructLayout(LayoutKind.Sequential)]
    public struct Rect {
        public int L, T, R, B;
    }
    public static string Text(IntPtr w) {
        var b = new StringBuilder(1024);
        SendMessage(w, 0x000d, (IntPtr)b.Capacity, b);
        return b.ToString();
    }
}
'@
$jobs = Get-Content -LiteralPath $Fixture -Raw -Encoding UTF8 | ConvertFrom-Json
$guiMutex = New-Object Threading.Mutex($false, 'Local\obfusDailyCrackmeGuiAcceptance')
$ownsMutex = $false
try {
    try {
        $ownsMutex = $guiMutex.WaitOne(120000)
    } catch [Threading.AbandonedMutexException] {
        $ownsMutex = $true
    }
    if (!$ownsMutex) {
        throw 'Another GUI acceptance did not release its lock'
    }
    foreach ($job in $jobs) {
        $process = Start-Process -FilePath $job.exe -WindowStyle Hidden -PassThru
        try {
            $watch = [Diagnostics.Stopwatch]::StartNew()
            do {
                Start-Sleep -Milliseconds 100
                $process.Refresh()
                $window = [DailyGui]::Find($process.Id)
            } while ($window -eq [IntPtr]::Zero -and !$process.HasExited -and $watch.Elapsed.TotalSeconds -lt 20)
            if ($window -eq [IntPtr]::Zero) {
                throw 'GUI did not create a window'
            }
            [void][DailyGui]::ShowWindow($window, 4)
            $edit = [DailyGui]::GetDlgItem($window, 100)
            $button = [DailyGui]::GetDlgItem($window, 101)
            $status = [DailyGui]::GetDlgItem($window, 102)
            $about = [DailyGui]::GetDlgItem($window, 103)
            if ($edit -eq [IntPtr]::Zero -or $button -eq [IntPtr]::Zero -or $status -eq [IntPtr]::Zero -or $about -eq [IntPtr]::Zero) {
                throw 'GUI controls missing'
            }
            [DailyGui+Rect]$client = New-Object DailyGui+Rect
            if (![DailyGui]::GetClientRect($window, [ref]$client)) {
                throw "GetClientRect failed; window=$window; exited=$($process.HasExited)"
            }
            if (($client.R - $client.L) -ne 760 -or ($client.B - $client.T) -ne 460) {
                throw "Incorrect client dimensions: $($client.R-$client.L)x$($client.B-$client.T)"
            }
            function Check-Result([string]$text, [bool]$enter, [string]$expected) {
                [DailyGui]::SetText($status, '')
                [void][DailyGui]::SendMessage($edit, 0x000c, [IntPtr]::Zero, $text)
                if ($enter) {
                    [void][DailyGui]::PostMessage($edit, 0x0100, [IntPtr]13, [IntPtr]::Zero)
                } else {
                    [void][DailyGui]::SendMessage($button, 0x00f5, [IntPtr]::Zero, [IntPtr]::Zero)
                }
                $deadline = [Diagnostics.Stopwatch]::StartNew()
                do {
                    Start-Sleep -Milliseconds 50
                    $actual = [DailyGui]::Text($status)
                } while ($actual -ne $expected -and $deadline.Elapsed.TotalSeconds -lt 10)
                if ($actual -ne $expected) {
                    throw 'GUI verdict mismatch'
                }
            }
            Check-Result '' $false 'Incorrect password.'
            Check-Result $job.password $true 'Accepted.'
            Check-Result ($job.password + '!') $false 'Incorrect password.'
            Check-Result $job.password $false 'Accepted.'
            # Test native paste, preserving the user's clipboard and awaiting UI completion.
            $oldClipboard = New-Object Windows.Forms.DataObject
            $clipboardSource = [Windows.Forms.Clipboard]::GetDataObject()
            if ($clipboardSource) {
                foreach ($format in $clipboardSource.GetFormats($false)) {
                    $clipboardValue = $clipboardSource.GetData($format, $false)
                    if ($clipboardValue -is [Drawing.Image]) {
                        $clipboardValue = $clipboardValue.Clone()
                    }
                    if ($clipboardValue -is [IO.MemoryStream]) {
                        $clipboardValue = New-Object IO.MemoryStream (, $clipboardValue.ToArray())
                    }
                    if ($null -ne $clipboardValue) {
                        $oldClipboard.SetData($format, $false, $clipboardValue)
                    }
                }
            }
            try {
                [DailyGui]::SetText($status, '')
                [Windows.Forms.Clipboard]::SetText($job.password, [Windows.Forms.TextDataFormat]::Text)
                [DailyGui]::SetText($edit, '')
                [void][DailyGui]::PostMessage($edit, 0x0302, [IntPtr]::Zero, [IntPtr]::Zero)
                # Password edits may hide their text from a different process. Assert the real verdict.
                [Windows.Forms.Application]::DoEvents()
                Start-Sleep -Milliseconds 350
                [void][DailyGui]::SendMessage($button, 0x00f5, [IntPtr]::Zero, [IntPtr]::Zero)
                $pasteDeadline = [Diagnostics.Stopwatch]::StartNew()
                do {
                    Start-Sleep -Milliseconds 50
                    $pasteResult = [DailyGui]::Text($status)
                } while ($pasteResult -ne 'Accepted.' -and $pasteDeadline.Elapsed.TotalSeconds -lt 10)
                if ($pasteResult -ne 'Accepted.') {
                    throw 'Pasted password was not accepted'
                }
                # Unicode clipboard data and deliberate contention must also work without another click on Paste.
                [DailyGui]::SetText($status, '')
                [DailyGui]::SetText($edit, '')
                [Windows.Forms.Clipboard]::SetText($job.password, [Windows.Forms.TextDataFormat]::UnicodeText)
                $lockWatch = [Diagnostics.Stopwatch]::StartNew()
                do {
                    $locked = [DailyGui]::OpenClipboard([IntPtr]::Zero)
                    if (!$locked) {
                        Start-Sleep -Milliseconds 10
                    }
                } while (!$locked -and $lockWatch.ElapsedMilliseconds -lt 2000)
                if (!$locked) {
                    throw 'Cannot acquire clipboard for contention test'
                }
                try {
                    [void][DailyGui]::PostMessage($edit, 0x0302, [IntPtr]::Zero, [IntPtr]::Zero)
                    Start-Sleep -Milliseconds 100
                } finally {
                    [void][DailyGui]::CloseClipboard()
                }
                [Windows.Forms.Application]::DoEvents()
                Start-Sleep -Milliseconds 350
                [void][DailyGui]::SendMessage($button, 0x00f5, [IntPtr]::Zero, [IntPtr]::Zero)
                if ([DailyGui]::Text($status) -ne 'Accepted.') {
                    throw 'Paste did not recover from clipboard contention'
                }
            } finally {
                if ($oldClipboard.GetFormats().Length) {
                    [Windows.Forms.Clipboard]::SetDataObject($oldClipboard, $true, 10, 100)
                } else {
                    [Windows.Forms.Clipboard]::Clear()
                }
            }
            Check-Result ($job.password.Replace($job.password.Substring(0, 1), ' ')) $false 'Incorrect password.'
            $unicodeFirst = [char]([int][char]$job.password[0] + 0xfee0)
            Check-Result ([string]$unicodeFirst + $job.password.Substring(1)) $false 'Incorrect password.'
            foreach ($keyboardAbout in @($false, $true)) {
                [DailyGui]::SetText($status, '')
                if ($keyboardAbout) {
                    [void][DailyGui]::PostMessage($about, 0x0100, [IntPtr]13, [IntPtr]::Zero)
                } else {
                    [void][DailyGui]::PostMessage($window, 0x0111, [IntPtr]103, [IntPtr]::Zero)
                }
                $aboutWatch = [Diagnostics.Stopwatch]::StartNew()
                do {
                    Start-Sleep -Milliseconds 20
                    $dialog = [DailyGui]::Dialog($process.Id)
                } while ($dialog -eq [IntPtr]::Zero -and $aboutWatch.ElapsedMilliseconds -lt 5000)
                if ($dialog -eq [IntPtr]::Zero) {
                    throw 'About dialog missing'
                }
                $aboutText = [DailyGui]::ChildrenText($dialog)
                foreach ($expected in @('reverse-engineering gamification program', 'Patching the success message does not count.', '13:00 Moscow time', 'https://github.com/DosX-dev/obfus.h?', 'Yes', 'No')) {
                    if (!$aboutText.Contains($expected)) {
                        throw 'About text/buttons mismatch'
                    }
                }
                [void][DailyGui]::SendMessage([DailyGui]::GetDlgItem($dialog, 7), 0x00f5, [IntPtr]::Zero, [IntPtr]::Zero)
                $closeWatch = [Diagnostics.Stopwatch]::StartNew()
                while ([DailyGui]::IsWindow($dialog) -and $closeWatch.ElapsedMilliseconds -lt 5000) {
                    Start-Sleep -Milliseconds 20
                }
                if ([DailyGui]::IsWindow($dialog) -or [DailyGui]::Text($status) -ne '') {
                    throw 'About did not close or incorrectly invoked the password check'
                }
            }
            if ($job.screenshot) {
                [void][DailyGui]::SendMessage($status, 0x000c, [IntPtr]::Zero, '')
                [void][DailyGui]::SendMessage($edit, 0x000c, [IntPtr]::Zero, '')
                [DailyGui+Rect]$rect = New-Object DailyGui+Rect
                [void][DailyGui]::GetWindowRect($window, [ref]$rect)
                $bitmap = New-Object Drawing.Bitmap ($rect.R - $rect.L), ($rect.B - $rect.T)
                $graphics = [Drawing.Graphics]::FromImage($bitmap)
                $dc = $graphics.GetHdc()
                try {
                    if (![DailyGui]::PrintWindow($window, $dc, 0)) {
                        throw 'Screenshot capture failed'
                    }
                } finally {
                    $graphics.ReleaseHdc($dc)
                }
                $bitmap.Save($job.screenshot, [Drawing.Imaging.ImageFormat]::Png)
                $graphics.Dispose()
                $bitmap.Dispose()
            }
            [void][DailyGui]::PostMessage($window, 0x0010, [IntPtr]::Zero, [IntPtr]::Zero)
            if (!$process.WaitForExit(10000) -or $process.ExitCode -ne 0) {
                throw 'GUI failed to close cleanly'
            }
            Write-Output 'GUI_PASS'
        } finally {
            if (!$process.HasExited) {
                Stop-Process -Id $process.Id -Force
            }
            $process.Dispose()
        }
    }
} finally {
    if ($ownsMutex) {
        $guiMutex.ReleaseMutex()
    }
    $guiMutex.Dispose()
}
