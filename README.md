# Html-Versioning
HTML code version program using C++ VS2019

HTML Source Code Versioning Program
css, js link file versioning
ex) ./index.css -> ./index.css?ver=1.0.1
    ./header.js -> ./header.js?ver=1.0.2

 process sequence
 
- 1. read version.txt file
    - version.txt
      - ./index.css   1.0.0
      - ./header.js   1.0.2
- 2. read *.html file in current directory
- 3. parse all html file
- 4. find filename contain version.txt files
  - <link ... href=* >
  - <script ... src=* >
- 5. change css, js filename to <filename>+<?ver=version>
