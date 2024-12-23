
# Document Source Code

## Gnerating HTML Document

### Environment Variables

The following environment variables must be set before using the Makefile to compile the document:

+ **HTML_DEST_DIR** points to the directory in which the HTML will be installed.
+ **PATH** should include the directory with the `doxyrest` binary.

### Makefile targets

The top Makefile contains the following targets, each depending on the previous one.

+ **xml**: Calls `doxygen` to extract the inline doc from C++ headers.
+ **rst**: Calls `doxyrest` to yield reStructuredText files from XML files.
+ **html**: Calls `sphinx` to generate HTML files from generated reStructuredText files and manually written ones.
+ **install**: Copies the HTML files to `$HTML_DEST_DIR`.