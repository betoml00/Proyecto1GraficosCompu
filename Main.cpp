#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <cmath>
#include <list>
#include <iterator>
#include <vector>

using namespace std;

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 800;

//Shaders
const char* vertexShaderSource = "#version 330 core\n"
"layout (location = 0) in vec3 aPos;\n"
"void main()\n"
"{\n"
"   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
"}\0";
//Tri�ngulos acut�ngulos
const char* fragmentShaderSource0 = "#version 330 core\n"
"out vec4 FragColor;\n"
"void main()\n"
"{\n"
"   FragColor = vec4(0.0823f, 0.5607f, 0.4313f, 1.0f);\n"
"}\n\0";
//Tri�ngulos obtus�ngulos
const char* fragmentShaderSource1 = "#version 330 core\n"
"out vec4 FragColor;\n"
"void main()\n"
"{\n"
"   FragColor = vec4(0.8705f, 0.7686f, 0.2509f, 1.0f);\n"
"}\n\0";
//Blanco para los ojos
const char* fragmentShaderSource2 = "#version 330 core\n"
"out vec4 FragColor;\n"
"void main()\n"
"{\n"
"   FragColor = vec4(1.0f, 1.0f, 1.0f, 1.0f);\n"
"}\n\0";
//Negro para los ojos
const char* fragmentShaderSource3 = "#version 330 core\n"
"out vec4 FragColor;\n"
"void main()\n"
"{\n"
"   FragColor = vec4(0.0f, 0.0f, 0.0f, 1.0f);\n"
"}\n\0";
const char* fragmentShaderSource4 = "#version 330 core\n"
"out vec4 FragColor;\n"
"void main()\n"
"{\n"
"   FragColor = vec4(0.1823f, 0.2607f, 0.7313f, 1.0f);\n"
"}\n\0";
const char* fragmentShaderSource5 = "#version 330 core\n"
"out vec4 FragColor;\n"
"void main()\n"
"{\n"
"   FragColor = vec4(0.7f, 0.2f, 0.35f, 1.0f);\n"
"}\n\0";
//Nuevo tri incorporado
const char* fragmentShaderSource6 = "#version 330 core\n"
"out vec4 FragColor;\n"
"void main()\n"
"{\n"
"   FragColor = vec4(0.8705f, 0.7686f, 0.2509f, 1.0f);\n"
"}\n\0";

//N�mero a�reo
const float FI = 1.618033;
//Pi
const float PI = 3.141592;

//Longitud de los lados de los tri�ngulos iniciales
const float iniLado = 0.75f;

//Total de tri�ngulos en un momento dado
unsigned int numTriAcu = 10;
unsigned int numTriObtu = 0;

//N�mero de veces que se subdividen los tri�ngulos
unsigned int numCiclos = 4;

//Traslada la teselaci�n
const float tras[2] = { -0.25f, 0.0f };

//�ndice del tri�ngulo a omitir de la teselaci�n para dejar un espacio
const int TRI_A_OMITIR = 76;
int omitir_index = 0;
/*
* Posici�n del tri�ngulo vac�o:
* A: 0.398975, 0.0338139
* B: 0.463292, 0.122339
* C: 0.463292, -0.0547117
*/

//Posici�n y tama�o del tri�ngulo (acut�ngulo) que se mueve
float pos[2] = { 1.0f, 0.0338139f }; //V�rtice izquierdo
float ladoCorto = 0.109424;
float ladoLargo = 0.177051;;
//Las medidas del tri�ngulo (obtus�ngulo) nuevo son 0.109424 (lados cortos) y 0.177051

//
float direccion = 1;

//Control del tiempo
/*0: inicio
* 1: Tri. entra en escena
* 2: Tri. se para un momento
* 3: Tri. se mueve hacia la teselaci�n y choca
* 4: Tri. se para un momento
* 5: Tri. choca varias veces m�s
* 6: Tri. cae hasta el suelo
* 7: Tri. rota hasta quedar paralelo al suelo y se rompe
* 8: Nuevo tri. se queda inm�vil un momento
* 9: Nuevo tri. se endereza y le vuelven a salir ojos
* 10: Nuevo tri. sube
* 11: Nuevo tri. se incorpora a la teselaci�n y cambia de color
* 12: Pausa
* 13: La teselaci�n entera alterna colores
*/
//                          0     1     2     3     4     5     6     7     8     9     10    11    12    13
const float tiempos[14] = { 2.0f, 1.0f, 1.5f, 1.5f, 0.5f, 2.0f, 0.9f, 0.1f, 4.0f, 2.0f, 2.0f, 1.0f, 2.0f, 3.5f };
//const float tiempos[15] = {0.0f, 2.0f, 3.0f, 4.5f, 6.0f, 6.5f, 7.5f, 8.4f, 8.5f, 12.5f, 14.5f, 16.5f, 17.5f, 19.5f, 23.0f};
int tiempoIndex = 0;

//Tri�ngulos de Robinson
typedef struct RTri {
    int color; //0 o 1
    float a[3];
    float b[3];
    float c[3];
} RTri;

//N�mero de tri�ngulos usados para aproximar un c�rculo
unsigned const int TRI_POR_CIRC = 10;

//C�rculos
typedef struct Circ {
    int color; //0 o 1 (blanco o negro)
    //vector<float> lista_tri;
    float listaVert[TRI_POR_CIRC * 9];
} circ;

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);
void subdividir(list<RTri>& listaTri);
float rotX(float c0, float c1, float x, float y, float theta);
float rotY(float c0, float c1, float x, float y, float theta);
RTri crearTri(int color, float a0, float a1, float b0, float b1, float c0, float c1);
Circ crearCirc(int color, float x, float y, float radio);
void animar();

int main() {
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // glfw window creation
    // --------------------
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
    if (window == NULL) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // build and compile our shader program
    // ------------------------------------
    // vertex shader
    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);

    // fragment shaders
    unsigned int fragmentShader0 = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader0, 1, &fragmentShaderSource0, NULL);
    glCompileShader(fragmentShader0);

    unsigned int fragmentShader1 = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader1, 1, &fragmentShaderSource1, NULL);
    glCompileShader(fragmentShader1);

    unsigned int fragmentShader2 = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader2, 1, &fragmentShaderSource2, NULL);
    glCompileShader(fragmentShader2);

    unsigned int fragmentShader3 = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader3, 1, &fragmentShaderSource3, NULL);
    glCompileShader(fragmentShader3);

    unsigned int fragmentShader4 = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader4, 1, &fragmentShaderSource4, NULL);
    glCompileShader(fragmentShader4);

    unsigned int fragmentShader5 = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader5, 1, &fragmentShaderSource5, NULL);
    glCompileShader(fragmentShader5);

    unsigned int fragmentShader6 = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader6, 1, &fragmentShaderSource6, NULL);
    glCompileShader(fragmentShader6);

    // link shaders
    unsigned int shaderProgram0 = glCreateProgram();
    glAttachShader(shaderProgram0, vertexShader);
    glAttachShader(shaderProgram0, fragmentShader0);
    glLinkProgram(shaderProgram0);

    unsigned int shaderProgram1 = glCreateProgram();
    glAttachShader(shaderProgram1, vertexShader);
    glAttachShader(shaderProgram1, fragmentShader1);
    glLinkProgram(shaderProgram1);

    unsigned int shaderProgram2 = glCreateProgram();
    glAttachShader(shaderProgram2, vertexShader);
    glAttachShader(shaderProgram2, fragmentShader2);
    glLinkProgram(shaderProgram2);

    unsigned int shaderProgram3 = glCreateProgram();
    glAttachShader(shaderProgram3, vertexShader);
    glAttachShader(shaderProgram3, fragmentShader3);
    glLinkProgram(shaderProgram3);

    unsigned int shaderProgram4 = glCreateProgram();
    glAttachShader(shaderProgram4, vertexShader);
    glAttachShader(shaderProgram4, fragmentShader4);
    glLinkProgram(shaderProgram4);

    unsigned int shaderProgram5 = glCreateProgram();
    glAttachShader(shaderProgram5, vertexShader);
    glAttachShader(shaderProgram5, fragmentShader5);
    glLinkProgram(shaderProgram5);

    unsigned int shaderProgram6 = glCreateProgram();
    glAttachShader(shaderProgram6, vertexShader);
    glAttachShader(shaderProgram6, fragmentShader6);
    glLinkProgram(shaderProgram6);

    //
    unsigned int VBO[7], VAO[7], EBO[7]; //0: tri acu, 1: tri obtu, 2: circ blancos, 3: circ negros, 4: tri�ngulo que se mueve
    glGenVertexArrays(7, VAO);
    glGenBuffers(7, VBO);
    glGenBuffers(7, EBO);

    //----INICIALIZACI�N----
    //Lista de tri�ngulos
    list<RTri> listaTri;

    //Lista de c�rculos
    list<Circ> listaCirc;

    //V�rtices de los primeros 10 tri�ngulos
    float verticesIni[] = {
        tras[0], tras[1], 0.0f,  // center
        tras[0], tras[1] + iniLado, 0.0f,  // top
        tras[0] - iniLado * sin(-PI / 5),     tras[1] + iniLado * cos(-PI / 5), 0.0f, //36�
        tras[0] - iniLado * sin(-2 * PI / 5), tras[1] + iniLado * cos(-2 * PI / 5), 0.0f, //72�
        tras[0] - iniLado * sin(-3 * PI / 5), tras[1] + iniLado * cos(-3 * PI / 5), 0.0f, //108�
        tras[0] - iniLado * sin(-4 * PI / 5), tras[1] + iniLado * cos(-4 * PI / 5), 0.0f, //144�
        tras[0] - iniLado * sin(-PI),         tras[1] + iniLado * cos(-PI), 0.0f, //180�
        tras[0] - iniLado * sin(-6 * PI / 5), tras[1] + iniLado * cos(-6 * PI / 5), 0.0f, //216�
        tras[0] - iniLado * sin(-7 * PI / 5), tras[1] + iniLado * cos(-7 * PI / 5), 0.0f, //252�
        tras[0] - iniLado * sin(-8 * PI / 5), tras[1] + iniLado * cos(-8 * PI / 5), 0.0f, //288�
        tras[0] - iniLado * sin(-9 * PI / 5), tras[1] + iniLado * cos(-9 * PI / 5), 0.0f //324�
    };

    //�ndices de los primeros 10 tri�ngulos
    unsigned int indicesIni[] = {
        0, 1, 2,  //1
        0, 3, 2,  //2
        0, 3, 4,  //3
        0, 5, 4,  //4
        0, 5, 6,  //5
        0, 7, 6,  //6
        0, 7, 8,  //7
        0, 9, 8,  //8
        0, 9, 10, //9
        0, 1, 10  //10
    };

    //Inicializa la lista con los 10 tri�ngulos
    for (int i = 0; i < 10; i++) {
        RTri nuevoTri = crearTri(0, verticesIni[0], verticesIni[1], verticesIni[3 * indicesIni[3 * i + 1]], verticesIni[3 * indicesIni[3 * i + 1] + 1],
            verticesIni[3 * indicesIni[3 * i + 2]], verticesIni[3 * indicesIni[3 * i + 2] + 1]);
        listaTri.push_back(nuevoTri);
    }

    //Subdivide todos los tr�angulos en la lista, de acuerdo al n�mero de ciclos
    for (int i = 0; i < numCiclos; i++)
        subdividir(listaTri);

    /*
    //Inicializa la lista de c�rculos
    //Ojo izquierdo
    listaCirc.push_back(crearCircRelleno(0, pos[0] + 0.04f, pos[1] + 0.02f, 0.05f));
    listaCirc.push_back(crearCircRelleno(1, pos[0] + 0.02f, pos[1] + 0.04f, 0.03f));
    //Ojo derecho
    listaCirc.push_back(crearCircRelleno(0, pos[0] + 0.04f, pos[1] + 0.02f, 0.05f));
    listaCirc.push_back(crearCircRelleno(1, pos[0] + 0.02f, pos[1] + 0.04f, 0.03f));
    */

    //----V�RTICES (VBO)----
    vector<float> vertices0; //Lista de v�rt. acu
    vector<float> vertices1; //Lista de v�rt. obtu
    vector<float> vertices2; //Lista de circ. blancos
    vector<float> vertices3; //Lista de circ. negros
    vector<float> vertices4; //Lista de v�rtices del tri. que se mueve
    vector<float> vertices5; //Lista de v�rtices del tri. roto

    //Guarda los v�rtices de todos los tri�ngulos en vectores (de acuerdo a su color)
    for (auto const& triActual : listaTri) {
        if (omitir_index != TRI_A_OMITIR) {
            if (triActual.color == 0) {
                vertices0.push_back(triActual.a[0]);
                vertices0.push_back(triActual.a[1]);
                vertices0.push_back(0.0f);
                vertices0.push_back(triActual.b[0]);
                vertices0.push_back(triActual.b[1]);
                vertices0.push_back(0.0f);
                vertices0.push_back(triActual.c[0]);
                vertices0.push_back(triActual.c[1]);
                vertices0.push_back(0.0f);
            }
            else {
                vertices1.push_back(triActual.a[0]);
                vertices1.push_back(triActual.a[1]);
                vertices1.push_back(0.0f);
                vertices1.push_back(triActual.b[0]);
                vertices1.push_back(triActual.b[1]);
                vertices1.push_back(0.0f);
                vertices1.push_back(triActual.c[0]);
                vertices1.push_back(triActual.c[1]);
                vertices1.push_back(0.0f);
            }

            omitir_index++;
        }
        else {
            if (triActual.color == 0) {
                numTriAcu--;
            }
            else {
                numTriObtu--;
            }

            omitir_index++;

            cout << "a: " << triActual.a[0] << ", " << triActual.a[1] << '\n' << "b: " << triActual.b[0] << ", " << triActual.b[1]
                << '\n' << "c: " << triActual.c[0] << ", " << triActual.c[1];
            //cout << sqrt(pow(triActual.a[0] - triActual.b[0], 2) + pow(triActual.a[1] - triActual.b[1], 2)) << '\n';
            //cout << sqrt(pow(triActual.a[0] - triActual.c[0], 2) + pow(triActual.a[1] - triActual.c[1], 2)) << '\n';
            //cout << sqrt(pow(triActual.c[0] - triActual.b[0], 2) + pow(triActual.c[1] - triActual.b[1], 2)) << '\n';
        }
    }

    /*
    //Guarda los v�rtices de todos los c�rculos en vectores (de acuerdo a su color)
    for(auto const& circActual : listaCirc) {
        if(circActual.color == 0) {
            for(int i = 0; i < TRI_POR_CIRC * 9; i++)
                vertices2.push_back(circActual.lista_tri[i]);
        } else {
            for(int i = 0; i < TRI_POR_CIRC * 9; i++)
                vertices3.push_back(circActual.lista_tri[i]);
        }
    }
    */

    //----�NDICES (EBO)----
    //Indexan los v�rtices
    vector<int> indices0;
    vector<int> indices1;
    vector<int> indices2;
    vector<int> indices3;
    vector<int> indices4;
    vector<int> indices5;

    for (int i = 0; i < 3 * numTriAcu; i++)
        indices0.push_back(i);

    for (int i = 0; i < 3 * numTriObtu; i++)
        indices1.push_back(i);

    /*
    for(int i = 0; i < 3 * TRI_POR_CIRC * listaCirc.size(); i++)
        indices2.push_back(i);

    for(int i = 0; i < 3 * TRI_POR_CIRC * listaCirc.size(); i++)
        indices3.push_back(i);
    */

    //Cargar al buffer
    //Tri�ngulos acut�ngulos
    glBindVertexArray(VAO[0]);
    glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
    glBufferData(GL_ARRAY_BUFFER, vertices0.size() * sizeof(float), &vertices0[0], GL_STATIC_DRAW);
    //glBufferData(GL_ARRAY_BUFFER, vertices0.size() * sizeof(float), &vertices0[0], GL_DYNAMIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO[0]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices0.size() * sizeof(int), &indices0[0], GL_STATIC_DRAW);
    //BufferData(GL_ELEMENT_ARRAY_BUFFER, indices0.size() * sizeof(int), &indices0[0], GL_DYNAMIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    //Tri�ngulos obtus�ngulos
    glBindVertexArray(VAO[1]);
    glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
    glBufferData(GL_ARRAY_BUFFER, vertices1.size() * sizeof(float), &vertices1[0], GL_STATIC_DRAW);
    //glBufferData(GL_ARRAY_BUFFER, vertices1.size() * sizeof(float), &vertices1[0], GL_DYNAMIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO[1]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices1.size() * sizeof(int), &indices1[0], GL_STATIC_DRAW);
    //glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices1.size() * sizeof(int), &indices1[0], GL_DYNAMIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // note that this is allowed, the call to glVertexAttribPointer registered VBO as the vertex attribute's bound vertex buffer object so afterwards we can safely unbind
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // You can unbind the VAO afterwards so other VAO calls won't accidentally modify this VAO, but this rarely happens. Modifying other
    // VAOs requires a call to glBindVertexArray anyways so we generally don't unbind VAOs (nor VBOs) when it's not directly necessary.
    glBindVertexArray(0);

    RTri protag, triRoto;

    glfwSetTime(0.0f);

    // render loop
    // -----------
    while (!glfwWindowShouldClose(window)) {
        //Redibuja el tri. cada cuadro
        listaTri.clear();

        if (tiempoIndex < 7) {
            protag = crearTri(0, pos[0], pos[1],
                rotX(pos[0], pos[1], pos[0] + ladoLargo, pos[1], PI / 10), rotY(pos[0], pos[1], pos[0] + ladoLargo, pos[1], PI / 10),
                rotX(pos[0], pos[1], pos[0] + ladoLargo, pos[1], -PI / 10), rotY(pos[0], pos[1], pos[0] + ladoLargo, pos[1], -PI / 10));
        }
        else if (tiempoIndex == 7) {
            protag = crearTri(0, pos[0], pos[1],
                rotX(pos[0], pos[1], 0.654901f, -1.0f, PI / 5), rotY(pos[0], pos[1], 0.654901f, -1.0f, PI / 5),
                0.654901f, -1.0f);
        }
        else if (tiempoIndex == 8 && glfwGetTime() < 1.0f / 60.0f) {
            RTri temp = crearTri(0, protag.a[0], protag.a[1], protag.c[0], protag.c[1], protag.b[0], protag.b[1]);
            listaTri.push_back(temp);
            subdividir(listaTri);

            pos[0] = listaTri.back().a[0];
            pos[1] = listaTri.back().a[1];
            protag = crearTri(0, pos[0], pos[1], listaTri.back().b[0], listaTri.back().b[1], listaTri.back().c[0], listaTri.back().c[1]);
            triRoto = listaTri.front();
        }
        else if (tiempoIndex == 9) {
            protag = crearTri(0, pos[0], pos[1], protag.b[0], protag.b[1],
                rotX(pos[0], pos[1], protag.b[0], protag.b[1], 3 * PI / 5), rotY(pos[0], pos[1], protag.b[0], protag.b[1], 3 * PI / 5));
        }
        else if (tiempoIndex > 9) {
            protag = crearTri(0, pos[0], pos[1],
                rotX(pos[0], pos[1], pos[0] + ladoCorto, pos[1], -3 * PI / 10), rotY(pos[0], pos[1], pos[0] + ladoCorto, pos[1], -3 * PI / 10),
                rotX(pos[0], pos[1], pos[0] + ladoCorto, pos[1], 3 * PI / 10), rotY(pos[0], pos[1], pos[0] + ladoCorto, pos[1], 3 * PI / 10));
        }

        vertices4.clear();
        indices4.clear();
        vertices5.clear();
        indices5.clear();

        vertices4.push_back(protag.a[0]);
        vertices4.push_back(protag.a[1]);
        vertices4.push_back(0.0f);
        vertices4.push_back(protag.b[0]);
        vertices4.push_back(protag.b[1]);
        vertices4.push_back(0.0f);
        vertices4.push_back(protag.c[0]);
        vertices4.push_back(protag.c[1]);
        vertices4.push_back(0.0f);

        for (int i = 0; i < 3; i++)
            indices4.push_back(i);

        vertices5.push_back(triRoto.a[0]);
        vertices5.push_back(triRoto.a[1]);
        vertices5.push_back(0.0f);
        vertices5.push_back(triRoto.b[0]);
        vertices5.push_back(triRoto.b[1]);
        vertices5.push_back(0.0f);
        vertices5.push_back(triRoto.c[0]);
        vertices5.push_back(triRoto.c[1]);
        vertices5.push_back(0.0f);

        for (int i = 0; i < 3; i++)
            indices5.push_back(i);

        //Tri�ngulo que se mueve
        if (tiempoIndex < 12) {
            glBindVertexArray(VAO[4]);
            glBindBuffer(GL_ARRAY_BUFFER, VBO[4]);
            glBufferData(GL_ARRAY_BUFFER, vertices4.size() * sizeof(float), &vertices4[0], GL_STATIC_DRAW);
            //glBufferData(GL_ARRAY_BUFFER, vertices4.size() * sizeof(float), &vertices4[0], GL_DYNAMIC_DRAW);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO[4]);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices4.size() * sizeof(int), &indices4[0], GL_STATIC_DRAW);
            //glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices4.size() * sizeof(int), &indices4[0], GL_DYNAMIC_DRAW);
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
            glEnableVertexAttribArray(0);
        }
        else if (tiempoIndex == 12) {
            //Nuevo tri. incorporado
            glBindVertexArray(VAO[6]);
            glBindBuffer(GL_ARRAY_BUFFER, VBO[6]);
            glBufferData(GL_ARRAY_BUFFER, vertices4.size() * sizeof(float), &vertices4[0], GL_STATIC_DRAW);
            //glBufferData(GL_ARRAY_BUFFER, vertices4.size() * sizeof(float), &vertices4[0], GL_DYNAMIC_DRAW);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO[6]);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices4.size() * sizeof(int), &indices4[0], GL_STATIC_DRAW);
            //glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices4.size() * sizeof(int), &indices4[0], GL_DYNAMIC_DRAW);
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
            glEnableVertexAttribArray(0);
        }
        else {

        }

        //Tri�ngulo roto
        if (tiempoIndex > 7) {
            glBindVertexArray(VAO[5]);
            glBindBuffer(GL_ARRAY_BUFFER, VBO[5]);
            glBufferData(GL_ARRAY_BUFFER, vertices5.size() * sizeof(float), &vertices5[0], GL_STATIC_DRAW);
            //glBufferData(GL_ARRAY_BUFFER, vertices5.size() * sizeof(float), &vertices5[0], GL_DYNAMIC_DRAW);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO[5]);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices5.size() * sizeof(int), &indices5[0], GL_STATIC_DRAW);
            //glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices5.size() * sizeof(int), &indices5[0], GL_DYNAMIC_DRAW);
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
            glEnableVertexAttribArray(0);
        }

        //Ojos
        listaCirc.clear();
        vertices2.clear();
        vertices3.clear();
        indices2.clear();
        indices3.clear();

        if (tiempoIndex < 7) {
            //Ojo izquierdo
            listaCirc.push_back(crearCirc(0, pos[0] + 0.08f, pos[1] + 0.02f, 0.018f));
            listaCirc.push_back(crearCirc(1, pos[0] + 0.068f, pos[1] + 0.02f, 0.014f));
            //Ojo derecho
            listaCirc.push_back(crearCirc(0, pos[0] + 0.14f, pos[1] + 0.02f, 0.018f));
            listaCirc.push_back(crearCirc(1, pos[0] + 0.128f, pos[1] + 0.02f, 0.014f));

            for (auto const& circActual : listaCirc) {
                if (circActual.color == 0) {
                    for (int i = 0; i < TRI_POR_CIRC * 9; i++)
                        vertices2.push_back(circActual.listaVert[i]);
                }
                else {
                    for (int i = 0; i < TRI_POR_CIRC * 9; i++)
                        vertices3.push_back(circActual.listaVert[i]);
                }
            }

            for (int i = 0; i < 3 * TRI_POR_CIRC * listaCirc.size() / 2; i++)
                indices2.push_back(i);

            for (int i = 0; i < 3 * TRI_POR_CIRC * listaCirc.size() / 2; i++)
                indices3.push_back(i);

            //C�rculos blancos
            glBindVertexArray(VAO[2]);
            glBindBuffer(GL_ARRAY_BUFFER, VBO[2]);
            glBufferData(GL_ARRAY_BUFFER, vertices2.size() * sizeof(float), &vertices2[0], GL_STATIC_DRAW);
            //glBufferData(GL_ARRAY_BUFFER, vertices2.size() * sizeof(float), &vertices2[0], GL_DYNAMIC_DRAW);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO[2]);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices2.size() * sizeof(int), &indices2[0], GL_STATIC_DRAW);
            //glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices2.size() * sizeof(int), &indices2[0], GL_DYNAMIC_DRAW);
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
            glEnableVertexAttribArray(0);

            //glClearBufferData(GL_ARRAY_BUFFER, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), &vertices2[0]);
            //glClearBufferData(GL_ELEMENT_ARRAY_BUFFER, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), &indices2[0]);

            //C�rculos negros
            glBindVertexArray(VAO[3]);
            glBindBuffer(GL_ARRAY_BUFFER, VBO[3]);
            glBufferData(GL_ARRAY_BUFFER, vertices3.size() * sizeof(float), &vertices3[0], GL_STATIC_DRAW);
            //glBufferData(GL_ARRAY_BUFFER, vertices3.size() * sizeof(float), &vertices3[0], GL_DYNAMIC_DRAW);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO[3]);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices3.size() * sizeof(int), &indices3[0], GL_STATIC_DRAW);
            //glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices3.size() * sizeof(int), &indices3[0], GL_DYNAMIC_DRAW);
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
            glEnableVertexAttribArray(0);
        }
        else if (tiempoIndex > 9 && tiempoIndex < 12) {
            //Ojo izquierdo
            listaCirc.push_back(crearCirc(0, pos[0] + 0.013f, pos[1] + 0.01f, 0.016f));
            listaCirc.push_back(crearCirc(1, pos[0] - 0.005f, pos[1] + 0.01f, 0.012f));
            //Ojo derecho
            listaCirc.push_back(crearCirc(0, pos[0] + 0.058f, pos[1] + 0.01f, 0.016f));
            listaCirc.push_back(crearCirc(1, pos[0] + 0.040f, pos[1] + 0.01f, 0.012f));

            for (auto const& circActual : listaCirc) {
                if (circActual.color == 0) {
                    for (int i = 0; i < TRI_POR_CIRC * 9; i++)
                        vertices2.push_back(circActual.listaVert[i]);
                }
                else {
                    for (int i = 0; i < TRI_POR_CIRC * 9; i++)
                        vertices3.push_back(circActual.listaVert[i]);
                }
            }

            for (int i = 0; i < 3 * TRI_POR_CIRC * listaCirc.size() / 2; i++)
                indices2.push_back(i);

            for (int i = 0; i < 3 * TRI_POR_CIRC * listaCirc.size() / 2; i++)
                indices3.push_back(i);

            //C�rculos blancos
            glBindVertexArray(VAO[2]);
            glBindBuffer(GL_ARRAY_BUFFER, VBO[2]);
            glBufferData(GL_ARRAY_BUFFER, vertices2.size() * sizeof(float), &vertices2[0], GL_STATIC_DRAW);
            //glBufferData(GL_ARRAY_BUFFER, vertices2.size() * sizeof(float), &vertices2[0], GL_DYNAMIC_DRAW);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO[2]);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices2.size() * sizeof(int), &indices2[0], GL_STATIC_DRAW);
            //glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices2.size() * sizeof(int), &indices2[0], GL_DYNAMIC_DRAW);
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
            glEnableVertexAttribArray(0);

            //C�rculos negros
            glBindVertexArray(VAO[3]);
            glBindBuffer(GL_ARRAY_BUFFER, VBO[3]);
            glBufferData(GL_ARRAY_BUFFER, vertices3.size() * sizeof(float), &vertices3[0], GL_STATIC_DRAW);
            //glBufferData(GL_ARRAY_BUFFER, vertices3.size() * sizeof(float), &vertices3[0], GL_DYNAMIC_DRAW);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO[3]);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices3.size() * sizeof(int), &indices3[0], GL_STATIC_DRAW);
            //glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices3.size() * sizeof(int), &indices3[0], GL_DYNAMIC_DRAW);
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
            glEnableVertexAttribArray(0);
        }

        // input
        // -----
        processInput(window);

        //Control de tiempos
        if (tiempoIndex < sizeof(tiempos) / sizeof(float)) {
            if (glfwGetTime() <= tiempos[tiempoIndex]) {
                animar();
            }
            else {
                cout << "Intervalo " << tiempoIndex << ": " << protag.a[0] << ", " << protag.a[1] << "; " << protag.b[0] << ", "
                    << protag.b[1] << "; " << protag.c[0] << ", " << protag.c[1] << '\n';

                glfwSetTime(0.0f);
                tiempoIndex++;
            }
        }
        else {
            glfwTerminate();
            return 0;
        }

        // render
        // ------
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        //Tri�ngulos acut�ngulos
        glUseProgram(shaderProgram0);
        glBindVertexArray(VAO[0]);
        glDrawElements(GL_TRIANGLES, 9 * numTriAcu, GL_UNSIGNED_INT, 0);

        //Tri�ngulos obtus�ngulos
        glUseProgram(shaderProgram1);
        glBindVertexArray(VAO[1]);
        glDrawElements(GL_TRIANGLES, 9 * numTriObtu, GL_UNSIGNED_INT, 0);

        //Tri�ngulo que se mueve
        glUseProgram(shaderProgram4);
        glBindVertexArray(VAO[4]);
        glDrawElements(GL_TRIANGLES, 9, GL_UNSIGNED_INT, 0);

        //C�rculos blancos
        glUseProgram(shaderProgram2);
        glBindVertexArray(VAO[2]);
        glDrawElements(GL_TRIANGLES, 9 * TRI_POR_CIRC * listaCirc.size() / 2, GL_UNSIGNED_INT, 0);

        //C�rculos negros
        glUseProgram(shaderProgram3);
        glBindVertexArray(VAO[3]);
        glDrawElements(GL_TRIANGLES, 9 * TRI_POR_CIRC * listaCirc.size() / 2, GL_UNSIGNED_INT, 0);

        //Tri�ngulo roto
        glUseProgram(shaderProgram5);
        glBindVertexArray(VAO[5]);
        glDrawElements(GL_TRIANGLES, 9, GL_UNSIGNED_INT, 0);

        //Nuevo tri incorporado
        glUseProgram(shaderProgram6);
        glBindVertexArray(VAO[6]);
        glDrawElements(GL_TRIANGLES, 9, GL_UNSIGNED_INT, 0);

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // optional: de-allocate all resources once they've outlived their purpose:
    // ------------------------------------------------------------------------
    glDeleteVertexArrays(7, VAO);
    glDeleteBuffers(7, VBO);
    glDeleteBuffers(7, EBO);
    glDeleteProgram(shaderProgram0);
    glDeleteProgram(shaderProgram1);
    glDeleteProgram(shaderProgram2);
    glDeleteProgram(shaderProgram3);
    glDeleteProgram(shaderProgram4);
    glDeleteProgram(shaderProgram5);
    glDeleteProgram(shaderProgram6);

    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
void processInput(GLFWwindow* window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}

//Crea un nuevo tri�ngulo de Robinson
RTri crearTri(int color, float a0, float a1, float b0, float b1, float c0, float c1) {
    RTri nuevoTri = {
        color,
        {a0, a1, 0.0f},
        {b0, b1, 0.0f},
        {c0, c1, 0.0f}
    };

    return nuevoTri;
}

//Subdivide un tri�ngulo de Robinson
void subdividir(list<RTri>& listaTri) {
    list<RTri> listaMod;

    for (auto const& triActual : listaTri) {
        if (triActual.color == 0) {
            //Tri�ngulo acut�ngulo
            float a0 = triActual.a[0];
            float a1 = triActual.a[1];
            float b0 = triActual.b[0];
            float b1 = triActual.b[1];
            float c0 = triActual.c[0];
            float c1 = triActual.c[1];

            //Calcula la posici�n del nuevo v�rtice
            //float p0 = c0 + (cos(PI / 5) * (a0 - c0) - sin(PI / 5) * (a1 - c1)) / FI;
            //float p1 = c1 + (sin(PI / 5) * (a0 - c0) + cos(PI / 5) * (a1 - c1)) / FI;
            float p0 = a0 + (b0 - a0) / FI;
            float p1 = a1 + (b1 - a1) / FI;

            //Crea los nuevos tri�ngulos
            RTri nuevoTriAcu = crearTri(0, c0, c1, p0, p1, b0, b1);
            RTri nuevoTriObtu = crearTri(1, p0, p1, c0, c1, a0, a1);

            //Inserta los tri�ngulos en la lista
            listaMod.push_back(nuevoTriAcu);
            listaMod.push_back(nuevoTriObtu);

            //Actualiza el total de tri�ngulos
            numTriObtu++;
        }
        else {
            //Tri�ngulo obtus�ngulo
            float a0 = triActual.a[0];
            float a1 = triActual.a[1];
            float b0 = triActual.b[0];
            float b1 = triActual.b[1];
            float c0 = triActual.c[0];
            float c1 = triActual.c[1];

            //Calcula la posici�n de los nuevos v�rtices
            //float r0 = a0 + (cos(-PI / 5) * (c0 - a0) - sin(-PI / 5) * (c1 - a1)) / FI;
            //float r1 = a1 + (sin(-PI / 5) * (c0 - a0) + cos(-PI / 5) * (c1 - a1)) / FI;
            //float q0 = r0 + cos(PI / 5) * (a0 - r0) - sin(PI / 5) * (a1 - r1);
            //float q1 = r1 + sin(PI / 5) * (a0 - r0) + cos(PI / 5) * (a1 - r1);

            float r0 = b0 + (c0 - b0) / FI;
            float r1 = b1 + (c1 - b1) / FI;
            float q0 = b0 + (a0 - b0) / FI;
            float q1 = b1 + (a1 - b1) / FI;

            //Crea los tri�ngulos nuevos
            RTri nuevoTriAcu = crearTri(0, r0, r1, q0, q1, a0, a1);
            RTri nuevoTriObtu1 = crearTri(1, q0, q1, r0, r1, b0, b1);
            RTri nuevoTriObtu2 = crearTri(1, r0, r1, c0, c1, a0, a1);

            //Inserta los tri�ngulos en la lista
            listaMod.push_back(nuevoTriAcu);
            listaMod.push_back(nuevoTriObtu1);
            listaMod.push_back(nuevoTriObtu2);

            //Actualiza el total de tri�ngulos
            numTriAcu++;
            numTriObtu++;
        }
    }

    //Modifica la lista original
    listaTri = listaMod;
}

//Rota un punto respecto a otro y regresa la coordenada x
float rotX(float c0, float c1, float x, float y, float theta) {
    return c0 + (x - c0) * cos(theta) - (y - c1) * sin(theta);
}

//Rota un punto respecto a otro y regresa la coordenada y
float rotY(float c0, float c1, float x, float y, float theta) {
    return c1 + (x - c0) * sin(theta) + (y - c1) * cos(theta);
}

//Crea un c�rculo
Circ crearCirc(int color, float x, float y, float radio) {
    Circ nuevoCirc;
    nuevoCirc.color = color;
    float theta = 2 * PI / TRI_POR_CIRC;

    for (int i = 0; i < 9 * TRI_POR_CIRC; i += 9) {
        //Primer v�rt.
        nuevoCirc.listaVert[i] = x;
        nuevoCirc.listaVert[i + 1] = y;
        nuevoCirc.listaVert[i + 2] = 0.0f;

        //Segundo v�rt.
        nuevoCirc.listaVert[i + 3] = x - radio * sin(-i * theta);
        nuevoCirc.listaVert[i + 4] = y - radio * cos(-i * theta);
        nuevoCirc.listaVert[i + 5] = 0.0f;

        //Tercer v�rt.
        nuevoCirc.listaVert[i + 6] = x - radio * sin(-(i + 1) * theta);
        nuevoCirc.listaVert[i + 7] = y - radio * cos(-(i + 1) * theta);
        nuevoCirc.listaVert[i + 8] = 0.0f;
    }

    //for(int i = 0; i < 9 * TRI_POR_CIRC; i += 3)
        //std::cout << nuevoCirc.lista_tri[i] << ", " << nuevoCirc.lista_tri[i + 1] << ", " << nuevoCirc.lista_tri[i + 2] << "\n";

    return nuevoCirc;
}

void animar() {
    float inter = 60 * tiempos[tiempoIndex]; //Intervalo actual (en marcos)
    float g = 3;
    float a0, a1, b0, b1, c0, c1, theta;

    switch (tiempoIndex) {
    case 0:
        break;

    case 1:
        //Posici�n inicial: (1, 0.0338139)
        if (pos[0] >= 0.8) {
            //pos[0] -= 0.01;
        }

        pos[0] -= 0.2 / inter;

        //cout << inter << '\n';
        //cout << pos[0] << '\n';

        //Posici�n final: (0.8, 0.0338139)
        break;

    case 2:
        break;

    case 3:
        //Posici�n inicial: (0.8002, 0.0338139)
        pos[0] -= (0.8 - 0.398975) / inter;

        //Posici�n final: (0.398975, 0.0338139)
        break;

    case 4:
        break;

    case 5:
        //Posici�n inicial: (0.398975, 0.0338139)
        if (pos[0] < 0.398975) {
            direccion = 1.0f;
        }
        else if (pos[0] > 0.5) {
            direccion = -1.0;
        }

        pos[0] += direccion * 5.199 * (0.5 - 0.398975) / inter;
        //cout << pos[0] << '\n';

        /*
        * Posici�n del tri�ngulo vac�o:
        * A: 0.398975, 0.0338139
        * B: 0.463292, 0.122339
        * C: 0.463292, -0.0547117
        */

        //Posici�n final: (0.50298, 0.0338139)
        break;

    case 6:
        //Posici�n inicial: (0.50298, 0.0338139)

        if (pos[1] + ladoLargo * sin(-PI / 10) > -1) {
            pos[1] -= g * glfwGetTime() / inter;
        }
        else if (pos[1] + ladoLargo * sin(-PI / 10) < -1) {
            pos[1] = -ladoLargo * sin(-PI / 10) - 1;
        }

        //cout << "A: " << pos[0] << ", " << pos[1] << " B: " << pos[0] + ladoLargo * cos(PI / 10) << ", " << pos[1] + ladoLargo * sin(PI / 10)
        //<< " C: " << pos[0] + ladoLargo * cos(-PI / 10) << ", " << pos[1] + ladoLargo * sin(-PI / 10) << '\n';

        //Posici�n final: (0.486515, -0.945288)
        break;

    case 7:
        //Posici�n inicial: (0.486515, -0.945288)
        //(0.486515, -0.945288); (0.654901, -0.890576); (0.654901, -1)
        //(0.477849, -1       ); (0.621087, -0.895931); (0.654901, -1)
        theta = PI / (10 * inter);
        c0 = 0.654901;
        c1 = -1;
        a0 = rotX(c0, c1, pos[0], pos[1], theta);
        a1 = rotY(c0, c1, pos[0], pos[1], theta);

        pos[0] = a0;
        pos[1] = a1;

        //Posici�n final: (0.477849, -1)
        break;

    case 8:
        break;

    case 9:
        //Posici�n inicial: (0.477849, -1)
        theta = -7 * PI / (10 * (inter - 0.5));

        b0 = 0.621087;
        b1 = -0.895931;

        a0 = rotX(b0, b1, pos[0], pos[1], theta);
        a1 = rotY(b0, b1, pos[0], pos[1], theta);

        pos[0] = a0;
        pos[1] = a1;

        //Posici�n final: (0.555644, -0.806999)
        break;

    case 10:
        // Posici�n inicial: (0.555644, -0.806999)

        pos[1] += (0.806999 + 0.0338139) / (inter - 0.5);

        // Posici�n final: (0.555644, 0.0338139)
        break;

    case 11:

        // Posici�n inicial: (0.555644, 0.0338139)

        pos[0] -= (0.555644 - 0.398975) / (inter - 0.5);

        // Posici�n final: (0.555644, 0.0338139)

        break;

    case 12:
        break;

    case 13:
        break;

    default:
        std::cout << "�ndice de tiempo inv�lido" << std::endl;
        glfwTerminate();
        break;
    }
}
