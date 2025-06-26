# GSoC Blog Post Scraper

A lightweight C++ command-line tool that scrapes Google Summer of Code blog posts from the official Google Open Source Blog and outputs them in JSON format. Perfect for automation, data analysis, or integration into other applications.

## Features

- **Fast Scraping**: Efficiently fetches GSoC blog posts using libcurl
- **JSON Output**: Clean, structured JSON output for easy parsing
- **Cross-platform**: Works on Windows, Linux, and macOS
- **Error Handling**: Robust error handling for network and parsing issues
- **Minimal Dependencies**: Only requires libcurl and libxml2

## Output Format

The tool outputs a JSON array of blog posts:
```json
[
  {
    "date": "Thursday, May 8, 2025",
    "title": "GSoC 2025: We have our Contributors!",
    "url": "https://opensource.googleblog.com/2025/05/gsoc-2025-we-have-our-contributors.html"
  },
  {
    "date": "Monday, March 24, 2025",
    "title": "Google Summer of Code 2025 Contributor Applications Now Open!",
    "url": "https://opensource.googleblog.com/2025/03/google-summer-of-code-2025-contributor.html"
  }
]
```

## Prerequisites

### Linux (Ubuntu/Debian)
```bash
sudo apt-get update
sudo apt-get install build-essential
sudo apt-get install libcurl4-openssl-dev
sudo apt-get install libxml2-dev
```

### Linux (CentOS/RHEL/Fedora)
```bash
# CentOS/RHEL
sudo yum install gcc-c++ make
sudo yum install libcurl-devel
sudo yum install libxml2-devel

# Fedora
sudo dnf install gcc-c++ make
sudo dnf install libcurl-devel
sudo dnf install libxml2-devel
```

### macOS
```bash
# Install Homebrew if you haven't already
/bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"

# Install dependencies
brew install curl
brew install libxml2
brew install pkg-config
```

### Windows

#### Option 1: Using vcpkg (Recommended)
```bash
# Install vcpkg
git clone https://github.com/Microsoft/vcpkg.git
cd vcpkg
./bootstrap-vcpkg.bat

# Install dependencies
vcpkg install curl:x64-windows
vcpkg install libxml2:x64-windows
```

#### Option 2: Manual Installation
- Download and install [libcurl for Windows](https://curl.se/windows/)
- Download and install [libxml2 for Windows](http://xmlsoft.org/downloads.html)
- Ensure libraries are in your system PATH

## Installation

### Clone the Repository
```bash
git clone https://github.com/life2harsh/gsoc-scraper.git
cd gsoc-blog-scraper
```

### Build Instructions

#### Linux/macOS
```bash
# Compile with pkg-config (recommended)
g++ -std=c++11 -o gsoc_scraper main.cpp `pkg-config --cflags --libs libcurl libxml-2.0`

# Alternative compilation (if pkg-config is not available)
g++ -std=c++11 -o gsoc_scraper main.cpp -lcurl -lxml2 -I/usr/include/libxml2
```

#### Windows (Visual Studio)
1. Open Visual Studio Command Prompt
2. If using vcpkg:
```bash
cl /EHsc main.cpp /I"path\to\vcpkg\installed\x64-windows\include" /link /LIBPATH:"path\to\vcpkg\installed\x64-windows\lib" libcurl.lib libxml2.lib
```

#### Windows (MinGW)
```bash
g++ -std=c++11 -o gsoc_scraper.exe main.cpp -lcurl -lxml2 -I/mingw64/include/libxml2
```

### Using CMake (Cross-platform)

Create a `CMakeLists.txt` file:
```cmake
cmake_minimum_required(VERSION 3.10)
project(GSoCBlogScraper)

set(CMAKE_CXX_STANDARD 11)

find_package(PkgConfig REQUIRED)
pkg_check_modules(LIBXML2 REQUIRED libxml-2.0)
find_package(CURL REQUIRED)

add_executable(gsoc_scraper main.cpp)

target_link_libraries(gsoc_scraper ${CURL_LIBRARIES} ${LIBXML2_LIBRARIES})
target_include_directories(gsoc_scraper PRIVATE ${CURL_INCLUDE_DIRS} ${LIBXML2_INCLUDE_DIRS})
target_compile_options(gsoc_scraper PRIVATE ${LIBXML2_CFLAGS_OTHER})
```

Then build:
```bash
mkdir build && cd build
cmake ..
make
```

## Usage

### Basic Usage
```bash
./gsoc_scraper
```

The program will:
1. Fetch the GSoC blog posts from Google's Open Source Blog
2. Parse the HTML content
3. Extract post titles, dates, and URLs
4. Output the results in JSON format to stdout

### Redirect Output to File
```bash
./gsoc_scraper > gsoc_posts.json
```

### Integration with Other Tools

#### Use with jq for parsing
```bash
# Get only post titles
./gsoc_scraper | jq '.[].title'

# Get posts from specific date
./gsoc_scraper | jq '.[] | select(.date | contains("2025"))'

# Count total posts
./gsoc_scraper | jq 'length'
```

#### Use in shell scripts
```bash
#!/bin/bash
posts=$(./gsoc_scraper)
echo "Found $(echo $posts | jq 'length') GSoC posts"
```

#### Python integration
```python
import subprocess
import json

result = subprocess.run(['./gsoc_scraper'], capture_output=True, text=True)
posts = json.loads(result.stdout)

for post in posts:
    print(f"{post['date']}: {post['title']}")
```

## Error Handling

The program handles various error conditions:

- **Network errors**: If the HTTP request fails, curl error messages are displayed
- **Parsing errors**: If HTML parsing fails, appropriate error messages are shown
- **XPath errors**: If the blog structure changes, XPath query failures are handled

### Common Error Messages

- `curl_easy_perform failed: [error]` - Network connectivity issues
- `Failed to parse HTML` - Invalid HTML response
- `XPath query fail` - Blog structure may have changed

## Customization

### Modify Target URL
Change the target URL in the source code:
```cpp
string docStr = get_request("https://opensource.googleblog.com/search/label/gsoc");
```

### Adjust XPath Selectors
If Google changes their blog structure, update these XPath expressions:
```cpp
xmlXPathObjectPtr dateNodes = get_nodes(htmldoc, BAD_CAST "//h4[@class='date-header']/span");
xmlXPathObjectPtr postNodes = get_nodes(htmldoc, BAD_CAST "//h3[@class='post-title entry-title']/a");
```

### Could also make this as a general-use tool by making a dedicated GUI and adding input fields for URL and tags to scrape data from.

## Performance

- **Memory Usage**: Approximately 1-5 MB depending on content size
- **Execution Time**: Typically 1-3 seconds depending on network speed
- **Network Usage**: Downloads ~50-200 KB of HTML content

## Dependencies Documentation

### libcurl
- **Purpose**: HTTP client library for fetching web content
- **Version**: 7.0+ (latest recommended)
- **License**: MIT-style license
- **Documentation**: https://curl.se/libcurl/

### libxml2
- **Purpose**: XML/HTML parsing and XPath queries
- **Version**: 2.6+ (latest recommended)  
- **License**: MIT License
- **Documentation**: http://xmlsoft.org/

## Troubleshooting

### Build Issues

1. **Missing headers**: Ensure development packages are installed
   ```bash
   # Ubuntu/Debian
   sudo apt-get install libcurl4-openssl-dev libxml2-dev
   ```

2. **Linking errors**: Check library paths and ensure libraries are installed
   ```bash
   # Check if libraries are available
   pkg-config --libs libcurl libxml-2.0
   ```

3. **Windows compilation**: Make sure to link against the correct runtime libraries

### Runtime Issues

1. **Network timeouts**: Check internet connection and firewall settings
2. **SSL certificate errors**: Update your system's CA certificates
3. **Empty output**: The blog structure may have changed; check XPath selectors

### Development Guidelines

- Follow C++11 standards
- Add error handling for new features
- Update documentation for any changes
- Test on multiple platforms when possible
- Make sure you have all the required libraries pre-installed.

## License

This project is licensed under the GNU License - see the [LICENSE](LICENSE) file for details.

## Support

For issues and questions:
1. Check existing [issues](https://github.com/life2harsh/gsoc-scraper/issues)
2. Create a new issue with:
   - Your operating system and version
   - Compiler and version used
   - Complete error messages
   - Steps to reproduce the problem
