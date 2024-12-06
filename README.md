# Project Configuration Instructions

/local.properties
- ```Change cmake.dir=C\:\\Users\\Admin\\AppData\\Local\\Android\\Sdk\\cmake\\3.10.2.4988404```
- ```Change sdk.dir=C\:\\Users\\Admin\\AppData\\Local\\Android\\Sdk```

/build.gradle
- ```Add mavenCentral() -> buildscript{...} -> repositories{...}```
- ```Add mavenCentral() -> allprojects{...} -> repositories{...}```
- ```Change classpath 'com.android.tools.build:gradle:8.0.0'```

/gradle-wrapper.properties
- ```Add org.gradle.jvmargs=--add-opens java.base/java.io=ALL-UNNAMED```
- ```Change distributionUrl=https\://services.gradle.org/distributions/gradle-8.0-all.zip```

/app/build.gradle
- ```Add namespace "com.tencent.yolov8ncnn"```
- ```Add ndkVersion '26.2.11394342'```
