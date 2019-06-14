# 技术

* OpenGL允许我们同时绑定多个缓冲，只要它们是不同的缓冲类型

  ```c++
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
  // GL_STATIC_DRAW ：数据不会或几乎不会改变。
  // GL_DYNAMIC_DRAW：数据会被改变很多。
  // GL_STREAM_DRAW ：数据每次绘制时都会改变。
  ```

* shader

  ~~~c++
  string vertex_shader_source = R"====(
  #version 330 core
  layout (location = 0) in vec3 aPos;
  layout (location = 1) in vec3 aColor;
  layout (location = 2) in vec2 aTexCoord;
  
  out vec3 ourColor;
  out vec2 TexCoord;
  
  void main()
  {
      gl_Position = vec4(aPos, 1.0);
      ourColor = aColor;
      TexCoord = aTexCoord;
  }
  )====";
  
  string frag_shader_source = R"====(
  #version 330 core
  out vec4 FragColor;
  
  in vec3 ourColor;
  in vec2 TexCoord;
  
  // texture samplers
  uniform sampler2D texture1;
  uniform sampler2D texture2;
  
  void main()
  {
  	// linearly interpolate between both textures (80% container, 20% awesomeface)
  	FragColor = mix(texture(texture1, TexCoord), texture(texture2, TexCoord), 0.2);
  }
  )====";
  
    
  // vertex shader
  GLuint vertexShader;
  vertexShader = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vertexShader, 1, &vertex_shader_source, NULL);
  glCompileShader(vertexShader);
  // error check
  glGetShaderiv(VertexShaderID, GL_COMPILE_STATUS, &Result);
  glGetShaderiv(VertexShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
  if ( InfoLogLength > 0 ){
    std::vector<char> VertexShaderErrorMessage(InfoLogLength+1);
    glGetShaderInfoLog(VertexShaderID, InfoLogLength, NULL, &VertexShaderErrorMessage[0]);
  		printf("%s\n", &VertexShaderErrorMessage[0]);
  }
  
  // fragment shader
  unsigned int fragmentShader;
  fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fragmentShader, 1, &frag_shader_source, NULL);
  glCompileShader(fragmentShader);
  // link
  unsigned int shaderProgram;
  shaderProgram = glCreateProgram();
  glAttachShader(shaderProgram, vertexShader);
  glAttachShader(shaderProgram, fragmentShader);
  glLinkProgram(shaderProgram);
  // error check 参考vertex shader
  // 激活
  glUseProgram(shaderProgram);
  glDeleteShader(vertexShader);
  glDeleteShader(fragmentShader);
  ~~~

* 数据

  ```c++
  std::vector<glm::vec3> indexed_vertices;
  GLuint vertexbuffer;
  // vao = {vbo} + {ebo(ibo)}：vertex array = {vectex buffer} + {element buffer}
  glGenBuffers(1, &vertexbuffer);
  glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
  glBufferData(GL_ARRAY_BUFFER, indexed_vertices.size() * sizeof(glm::vec3), &indexed_vertices[0], GL_STATIC_DRAW);
  // ebo
  float vertices[] = {
      0.5f, 0.5f, 0.0f,   // 右上角
      0.5f, -0.5f, 0.0f,  // 右下角
      -0.5f, -0.5f, 0.0f, // 左下角
      -0.5f, 0.5f, 0.0f   // 左上角
  };
  unsigned int indices[] = { // 注意索引从0开始! 
      0, 1, 3, // 第一个三角形
      1, 2, 3  // 第二个三角形
  };
  GLuint elementbuffer;
  glGenBuffers(1, &elementbuffer);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementbuffer);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned short), &indices[0] , GL_STATIC_DRAW);
  // 1. 设置顶点属性指针
  glEnableVertexAttribArray(0);
  glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
  // 2. 当我们渲染一个物体时要使用着色器程序
  glUseProgram(shaderProgram);
  // 3. 绘制物体
  glDrawArrays(GL_TRIANGLES, 0, 3);
  glDrawElements(
  			GL_TRIANGLES,      // mode
  			indices.size(),    // count
  			GL_UNSIGNED_SHORT,   // type
  			(void*)0           // element array buffer offset
  		);
  glDisableVertexAttribArray(0);
  glDisableVertexAttribArray(1);
  ```

* uniform类似于全局变量。在查询位置`glGetUniformLocation`前不需要调用`glUseProgram`，但在设置时，得加上

* texutre：[源代码](https://learnopengl.com/code_viewer_gh.php?code=src/1.getting_started/4.2.textures_combined/textures_combined.cpp)；[教程](https://learnopengl-cn.github.io/01%20Getting%20started/06%20Textures/)。

  ```c++
  /*************** 构造纹理 *************/
  unsigned int texture;
  glGenTextures(1, &texture);
  glBindTexture(GL_TEXTURE_2D, texture);
  // 环绕方式
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
  // 边缘色——optional
  float borderColor[] = { 1.0f, 1.0f, 0.0f, 1.0f };
  glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
  // 插值方式
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  // 多级渐进——只在缩小时有用
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);//
  // 加载&生成纹理
  stbi_set_flip_vertically_on_load(true);	// 纹理上下翻转问题
  unsigned char *data = stbi_load("container.jpg", &width, &height, &nrChannels, 0);
  if (data) {
  	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
    // 生成多级渐进纹理——optional
  	glGenerateMipmap(GL_TEXTURE_2D);
  }
  /*************** 绘制纹理 *************/
  // VAOs = {VBOs + EBO}
  glActiveTexture(GL_TEXTURE0);	// 纹理单元0默认总是被激活，所以可以略过
  glBindTexture(GL_TEXTURE_2D, texture1);
  glBindVertexArray(VAO);
  glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
  ourShader.use();
  glBindVertexArray(VAO);
  glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
  glDeleteVertexArrays(1, &VAO);
  glDeleteBuffers(1, &VBO);
  glDeleteBuffers(1, &EBO);
  
  /*************** 删除纹理 *************/
  glDeleteTextures(1, &adaptor_mask_texture_);
  ```

* 变换

  * w分量(齐次坐标)为0时，是方向向量：因为其不能位移

  * space: local, world, view, clip, creen

    ```c++
    // vertext shader
    R"==(
    uniform mat4 MVP;
    uniform mat4 V;
    uniform mat4 M;
    )==";
    
    void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
    	if(firstMouse) {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
      }
        float xoffset = xpos - lastX;
        float yoffset = lastY - ypos; 
        lastX = xpos;
        lastY = ypos;
    
        float sensitivity = 0.05;
        xoffset *= sensitivity;
        yoffset *= sensitivity;
    
        yaw   += xoffset;
        pitch += yoffset;
    
        if(pitch > 89.0f) pitch = 89.0f;
        if(pitch < -89.0f) pitch = -89.0f;
    
        glm::vec3 front;
        front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
        front.y = sin(glm::radians(pitch));
        front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
        cameraFront = glm::normalize(front);
    }
    
    glm::mat4 ProjectionMatrix = getProjectionMatrix();
    glm::mat4 view = getViewMatrix();
    view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp); 
    glm::mat4 model = glm::mat4(1.0);
    glm::mat4 MVP = ProjectionMatrix * ViewMatrix * ModelMatrix;
    // 每次渲染时进行——可能每次mvp都有变动
    int model_loc = glGetUniformLocation(ourShader.ID, "model"));
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
    ```

  * Z缓冲：深度信息。

    ```c++
    glEnable(GL_DEPTH_TEST);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);	// 每一帧清除
    ```

    