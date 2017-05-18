# WindowsFolderPickerFix
Do you recognize this folder selection dialog?

![Old dialog](https://i.imgur.com/iG3VVrK.png)

It's terrible. It dates back to Windows XP and misses several key UI components.
 - Shortcuts to common folders
 - Sorting
 - Search
 - Details about the folders
 - A new folder button
 - ...

Back when Windows Vista was released, a replacement was introduced that fixes these issues:

![Old dialog](https://i.imgur.com/hiMaJG3.png)

Unfortunately, some programs still use the old dialog. This project aims to fix that.
SHBrowseForFolder function calls are intercepted and replaced with calls to the newer IFileOpenDialog API.
When compatibility cannot be guaranteed (some features of the older dialog are not available on the newer one), the older dialog is used as a fallback.

This project uses minhook as its detouring library and WiX as installer.
