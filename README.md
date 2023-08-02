![Image](RParseX.png)

<div align="center">
RParseX ~ Blazing fast stealerlogs parser by Serialized
</div>

# RParseX

RParseX is a tool for extracting and processing data from password files found in specified directories. It scans the directories for password files, extracts relevant data, and organizes it based on target websites.

## Compatible Stealers
- Redline
- Racoon
- Doenerium
- Something missing? Open an Issue.

# Installation
```bash
git clone --recursive https://github.com/nak0823/RParseX.git
```
# Usage

```bash
RParseX.exe <path>
```
# Dependicies
- Microsoft Visual C++ Latest Redistributable

# How it works
- Scanning Directories: It locates all valid directories containing password files within the specified logs directory.

- Extracting Data: The tool reads each password file, extracts relevant data (username, password, and source), and stores it in a structured format.

- Organizing Data: RParseX organizes the extracted data based on the target websites and creates separate files for each target website containing the corresponding login information.

- Formatting Combo Data: The extracted combo data (username, password, and source) is formatted into a human-readable string for output.

- Timestamping: The tool timestamps the output files with the current date and time.




## License

[MIT](https://choosealicense.com/licenses/mit/)

