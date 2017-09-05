# jni-maven-guide
## 1. 创建maven项目
```bash
mvn archetype:generate \
    -DarchetypeGroupId=org.apache.maven.archetypes \
    -DarchetypeArtifactId=maven-archetype-quickstart \
    -DgroupId=<my.groupId> \
    -DartifactId=<my.artifactId>
```

## 2. 修改目录结构
> 只是个人习惯,如果不喜欢可自由发挥

1. 在项目根目录下建立clib,用来存放编译好的库文件
2. 在项目根目录下建立bin,用来项目启动脚本
3. 进入到src/main 新建c目录 -> src/main/c 用来存放c代码相关内容
```
-src
--main/c
--main/java
```

## 3. 添加maven插件
### java编译插件
将maven-compiler-plugin的compile目标(goal)附加到Lifecycle phas
e validate上,保证class编译第一步进行
```xml
<plugin>
    <groupId>org.apache.maven.plugins</groupId>
    <artifactId>maven-compiler-plugin</artifactId>
    <executions>
      <execution>
        <phase>validate</phase>
        <goals>
          <goal>compile</goal>
        </goals>
      </execution>
    </executions>
 </plugin>
```

### .h头文件生成
传统的方式是使用javah,如`javah MyClass`,现在采用maven插件的方式
将native-maven-plugin的javah目标绑定在phase generate-resour
ces上
```xml
<plugin>
    <groupId>org.codehaus.mojo</groupId>
    <artifactId>native-maven-plugin</artifactId>
    <executions>
      <execution>
        <id>javah</id>
        <phase>generate-resources</phase>
        <goals>
          <goal>javah</goal>
        </goals>
        <configuration>
          <!-- 头文件输出目录 ${basedir}为maven内置变量-->
          <javahOutputDirectory>${basedir}/src/main/c</javahOutputDirectory>
          <javahOutputFileName>learn_jni.h</javahOutputFileName>
          <!-- 需要编译成头文件的class,必须是全限定类名 -->
          <javahClassNames>
            <javahClassName>learn.jni.Sample01</javahClassName>
          </javahClassNames>
        </configuration>
      </execution>
    </executions>
 </plugin>
```

### 编写c代码并编译
根据生成的头文件,可以愉快地在src/main/c里写c代码了,这一步不需要任何java
或者maven的知识,写完后,使用Makefile或者Virtual Studio编译成动态库
linux下是.so,windows下是.dll,并将库文件放入clib目录下

### 复制动态库
添加与资源文件操作相关的插件maven-resources-plugin,在phase compile
阶段(当然可以选择其他生命周期,这里无所谓的,即使没有资源在clib中,也不会fail
)执行目标copy-resources,将动态库复制到target/classes(存放编译后.class
文件的目录),这样的好处是,打包的时候可以将动态库一起打包,不管是在编码阶段
还是发布后,都是直接可以在类加载路径下找到动态库,很方便,很统一(有点类似于java 
web项目).

**不要将动态库放在jar包里,因为Syetem.load没有提供InputStream
的重载方法,jar不属于文件系统目录,而Syetem.load参数只接受文件
系统的绝对路径的,放在jar包里会找不到动态库**
```xml
  <plugin>
    <groupId>org.apache.maven.plugins</groupId>
    <artifactId>maven-resources-plugin</artifactId>
    <executions>
      <execution>
        <id>copy-clib</id>
        <phase>compile</phase>
        <goals>
          <goal>copy-resources</goal>
        </goals>
        <configuration>
          <resources>
            <resource>
              <directory>clib</directory>
            </resource>
          </resources>
          <outputDirectory>${project.build.directory}/classes</outputDirectory>
        </configuration>
      </execution>
    </executions>
  </plugin>
```

### 打包发布项目
添加maven-assembly-plugin最大程度自定义地构架发布包
```xml
 <plugin>
    <artifactId>maven-assembly-plugin</artifactId>
    <version>3.1.0</version>
    <configuration>
      <descriptors>
        <descriptor>assembly.xml</descriptor>
      </descriptors>
    </configuration>
    <executions>
      <execution>
        <id>package</id>
        <phase>package</phase>
        <goals>
          <goal>single</goal>
        </goals>
      </execution>
    </executions>
 </plugin>
```

创建assembly.xml指示具体构建方式
> idea上 "http://maven.apache.org/ASSEMBLY/2.0.0"会
报"uri not register错误",直接"alt+enter"就可以解决:)

```xml
<assembly xmlns="http://maven.apache.org/ASSEMBLY/2.0.0"
          xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
          xsi:schemaLocation="http://maven.apache.org/ASSEMBLY/2.0.0 http://maven.apache.org/xsd/assembly-2.0.0.xsd">
    <id>bin</id>
    <formats>
        <format>tar.gz</format>
        <format>dir</format>
    </formats>

    <dependencySets>
        <dependencySet>
            <outputDirectory>/lib</outputDirectory>
            <useProjectArtifact>false</useProjectArtifact>
        </dependencySet>
    </dependencySets>

    <fileSets>
        <fileSet>
            <directory>${project.build.directory}/classes</directory>
            <outputDirectory>/classes</outputDirectory>
        </fileSet>
    </fileSets>

    <files>
        <file>
            <source>bin/start.sh</source>
            <outputDirectory>/</outputDirectory>
            <fileMode>0755</fileMode>
        </file>
    </files>
</assembly>
```

## 使用vscode编写c
> 更详细操作[coding C/C++ through vscode](https://code.visualstudio.com/docs/languages/cpp)
1. 使用vscode(July 2017 version 1.15)打开项目文件夹,安装c/c++插件(微软自提供的)
2. 在用户设置里setting.xml 设置`"C_Cpp.intelliSenseEngine": "Default"` 开启autocomplete等功能(Version 0.12.3: August 17, 2017)
3. 发现头文件learn_jni.h处`#include <jni.h>`有绿色下划线,将鼠标放上去后,左侧出现黄色灯泡,点击,选择第一个选项,自动在项目根目录下生成.vscode/c\_cpp\_properties.json,并引入了includePath(头文件目录)
4. 加入项目本身的头文件位置,如本项目,加入这后,就能轻松的ctrl+鼠标 转到定义了(选中,右键,里面有更多查看定义的方法与快捷键)
```json
{
  "name": "Linux",
  "includePath": [
      "/usr/include/c++/5.4.0",
      "/usr/include/x86_64-linux-gnu/c++/5.4.0",
      "/usr/local/include",
      "/usr/include",
      "/usr/include/x86_64-linux-gnu",
      "/usr/lib/gcc/x86_64-linux-gnu/5/include",
      "/usr/local/java/jdk1.8.0_101/include",
      "/usr/local/java/jdk1.8.0_101/include/linux",
      "${workspaceRoot}/src/main/c"
  ],
  "defines": [],
  "intelliSenseMode": "clang-x64",
  "browse": {
      "path": [
          "/usr/include/c++/5.4.0",
          "/usr/include/x86_64-linux-gnu/c++/5.4.0",
          "/usr/local/include",
          "/usr/include",
          "/usr/include/x86_64-linux-gnu",
          "/usr/lib/gcc/x86_64-linux-gnu/5/include",
          "/usr/local/java/jdk1.8.0_101/include",
          "/usr/local/java/jdk1.8.0_101/include/linux",
          "${workspaceRoot}/src/main/c"
      ],
      "limitSymbolsToIncludedHeaders": true,
      "databaseFilename": ""
  }
},
```
5. 建立自动build任务,由于我们已经有了Makefile文件,因此只需要vscode执行make就行了
`ctrl+shitf+p`打开命令框,输入Task:Run build task,生成tasks.json文件在.vscode下
修改[更具体的操作](https://code.visualstudio.com/docs/editor/tasks)
```json
{
    "version": "2.0.0",
    "tasks": [
        {
            "taskName": "build share library",
            "type": "shell",
            "command": "make",
            "args": [],
            "group": {
                "kind": "build",
                "isDefault": true
            }
        }
    ]
}
```
