//Define deshabilitaciones de advertencias de seguridad de Windows
#define _CRT_SECURE_NO_WARNINGS
#define WIN32_LEAN_AND_MEAN

#include "pch.h"
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <map>
#include <vector>
#include <algorithm>
#include <cstdlib>
#include <limits>
#include <iomanip>
#include <curl/curl.h>
#include <windows.h>
#include <msclr/marshal_cppstd.h>
#include <cctype>
#include "stdafx.h"
#include <nlohmann/json.hpp>
#include <fcntl.h>
#include <io.h>
#include <chrono>   
#include <thread>
#include <sql.h>
#include <sqlext.h>
#include <odbcinst.h>

#pragma comment(lib, "odbc32.lib") //Biblioteca ODBC    

//Biblioteca de manejo de datos
using namespace std;
using namespace System;
using namespace System::Data;
using namespace System::Data::SqlClient;
using namespace msclr::interop;
using namespace std::this_thread;
using namespace std::chrono;
using json = nlohmann::json;

//Configuracíón de conexión de SQL
#define DSN_NAME "SQLServerDSN"
#define USER "sa"
#define PASSWORD "Cacadeperico123!"
#define SERVER "VICTUS"
#define DATABASE "Chatdatabase"

// Definir códigos de color
#define COLOR_RESET   "\033[0m"
#define COLOR_RED     "\033[31m"
#define COLOR_GREEN   "\033[32m"
#define COLOR_YELLOW  "\033[33m"
#define COLOR_BLUE    "\033[34m"
#define COLOR_MAGENTA "\033[35m"
#define COLOR_CYAN    "\033[36m"
#define COLOR_WHITE   "\033[37m"

#define BG_BLUE       "\033[44m"

SQLHENV hEnv;
SQLHDBC hDbc;
SQLHSTMT hStmt;

map<string, string> conocimiento;

//Limpia pantalla
void limpiarPantalla() {
    system("cls");
}

void mostrarLinea() {
    cout << "===============================================\n";
}

//Pausa las funciones y regresa a la pantalla principal
void pausar() {
    cout << "\nPresione ENTER para continuar...";
    cin.ignore();
    cin.get();
}

//Escritura de datos recibidos en cURL
size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userp) {
    ((string*)userp)->append((char*)contents, size * nmemb);
    return size * nmemb;
}

//Soporte de UTF-8
void configurarConsolaUTF8() {
    _setmode(_fileno(stdout), _O_U8TEXT);
    _setmode(_fileno(stdin), _O_U8TEXT);
    _setmode(_fileno(stderr), _O_U8TEXT);
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
}

//Elimina espacios en blanco al inicio y final del string
string trim(const string& str) {
    const char* ws = " \t\n\r\f\v";
    size_t b = str.find_first_not_of(ws);
    if (b == string::npos) return "";
    size_t e = str.find_last_not_of(ws);
    return str.substr(b, e - b + 1);
}

//Función para cargar conocimiento desde el archivo conocimiento.txt
void cargarConocimiento(map<string, string>& conocimiento, const string& nombreArchivo) {
    ifstream archivo(nombreArchivo);
    if (!archivo.is_open()) {
        std::cout << "Error al abrir el archivo de conocimiento." << endl;
        return;
    }

    string linea;
    while (getline(archivo, linea)) {
        size_t separador = linea.find("|");
        if (separador != string::npos) {
            string pregunta = trim(linea.substr(0, separador));
            string respuesta = trim(linea.substr(separador + 1));
            conocimiento[pregunta] = respuesta;
        }
    }
    archivo.close();
}

// Funciones de conexión a la base de datos
bool conectarBD() {
    SQLRETURN ret;

    //Maneja funciones de SQL
    if (SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &hEnv) != SQL_SUCCESS) {
        cerr << "Error al asignar el handle de entorno" << endl;
        return false;
    }

    //Coloca los atributos de ODBC en SQL
    if (SQLSetEnvAttr(hEnv, SQL_ATTR_ODBC_VERSION, (void*)SQL_OV_ODBC3, 0) != SQL_SUCCESS) {
        cerr << "Error al establecer la versión ODBC" << endl;
        SQLFreeHandle(SQL_HANDLE_ENV, hEnv);
        return false;
    }

    //Coloca las funciones de conexión del servidor
    if (SQLAllocHandle(SQL_HANDLE_DBC, hEnv, &hDbc) != SQL_SUCCESS) {
        cerr << "Error al asignar el handle de conexión" << endl;
        SQLFreeHandle(SQL_HANDLE_ENV, hEnv);
        return false;
    }

    //Se conecta a la base de datos usando el autenticador de Windows
    SQLCHAR connStr[1024];
    sprintf_s((char*)connStr, sizeof(connStr),
        "DRIVER={SQL Server};SERVER=%s;DATABASE=%s;UID=%s;PWD=%s;",
        SERVER, DATABASE, USER, PASSWORD);

    ret = SQLDriverConnectA(hDbc, NULL, connStr, SQL_NTS, NULL, 0, NULL, SQL_DRIVER_COMPLETE);

    if (ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO) {
        SQLCHAR sqlState[1024] = "", message[1024] = "";
        SQLSMALLINT len;
        SQLGetDiagRecA(SQL_HANDLE_DBC, hDbc, 1, sqlState, NULL, message, sizeof(message), &len);
        cerr << "Error al conectar a la base de datos: " << message << endl;
        SQLFreeHandle(SQL_HANDLE_DBC, hDbc);
        SQLFreeHandle(SQL_HANDLE_ENV, hEnv);
        return false;
    }

    return true;
}

//Se desconecta de la base de datos
void desconectarBD() {
    SQLDisconnect(hDbc);
    SQLFreeHandle(SQL_HANDLE_DBC, hDbc);
    SQLFreeHandle(SQL_HANDLE_ENV, hEnv);
}

//Carga conocimientos desde SQL usando .NET
void cargarConocimientoDesdeBD(map<string, string>& conocimiento) {
    String^ cadenaConexion = "Data Source=VICTUS;Initial Catalog=Chatdatabase;User ID=sa;Password=Cacadeperico123!;";
    SqlConnection^ conexion = gcnew SqlConnection(cadenaConexion);

    std::cout << "Se cargaron " << conocimiento.size() << " preguntas desde la BD" << endl;

    try {
        conexion->Open();
        String^ consulta = "SELECT Pregunta, Respuesta FROM Conocimiento";
        SqlCommand^ comando = gcnew SqlCommand(consulta, conexion);
        SqlDataReader^ lector = comando->ExecuteReader();

        while (lector->Read()) {
            string pregunta = marshal_as<string>(lector["Pregunta"]->ToString());
            string respuesta = marshal_as<string>(lector["Respuesta"]->ToString());
            conocimiento[pregunta] = respuesta;
        }
        conexion->Close();
    }
    catch (Exception^ ex) {
        Console::WriteLine("Error al cargar desde BD: {0}", ex->Message);
    }
}

//Normaliza texto en trim y lowercase
string normalizarTexto(const string& texto) {
    string normalizado = trim(texto);
    transform(normalizado.begin(), normalizado.end(), normalizado.begin(), ::tolower);
    return normalizado;
}

// Función para dividir una frase en palabras individuales
vector<string> dividirPalabras(const string& frase) {
    vector<string> palabras;
    stringstream ss(normalizarTexto(frase));
    string palabra;
    while (ss >> palabra) {
        palabras.push_back(palabra);
    }
    return palabras;
}

// Función para buscar una coincidencia exacta en el conocimiento
string buscarExacto(const map<string, string>& conocimiento, const string& pregunta) {
    string preguntaNorm = normalizarTexto(pregunta);

    // Primero buscamos coincidencia exacta
    auto it = conocimiento.find(preguntaNorm);
    if (it != conocimiento.end()) {
        return it->second;
    }

    // Si no hay coincidencia exacta, buscamos con normalización
    for (const auto& par : conocimiento) {
        string claveNorm = normalizarTexto(par.first);
        if (preguntaNorm == claveNorm) {
            return par.second;
        }
    }

    return ""; // Retorna vacío si no encuentra
}

// Función para buscar por palabras clave con umbral mínimo
string buscarPorPalabrasClave(const map<string, string>& conocimiento, const string& pregunta) {
    vector<string> palabrasPregunta = dividirPalabras(pregunta);
    if (palabrasPregunta.empty()) return "";

    string mejorRespuesta = "";
    int maxCoincidencias = 0;
    float mejorPorcentaje = 0.0f;
    const float UMBRAL_MINIMO = 0.6f; // 60% de coincidencia requerida

    for (const auto& par : conocimiento) {
        vector<string> palabrasBase = dividirPalabras(par.first);
        if (palabrasBase.empty()) continue;

        int coincidencias = 0;
        for (const string& palabraPregunta : palabrasPregunta) {
            for (const string& palabraBase : palabrasBase) {
                if (palabraPregunta == palabraBase) {
                    coincidencias++;
                    break; // Evita contar múltiples coincidencias de la misma palabra
                }
            }
        }

        float porcentajeCoincidencia = static_cast<float>(coincidencias) / palabrasPregunta.size();

        // Solo consideramos si supera el umbral y es mejor que coincidencias anteriores
        if (porcentajeCoincidencia >= UMBRAL_MINIMO &&
            (porcentajeCoincidencia > mejorPorcentaje ||
                (porcentajeCoincidencia == mejorPorcentaje && coincidencias > maxCoincidencias))) {
            mejorPorcentaje = porcentajeCoincidencia;
            maxCoincidencias = coincidencias;
            mejorRespuesta = par.second;
        }
    }

    return mejorRespuesta;
}

// Función unificada de búsqueda
string buscarRespuesta(const map<string, string>& conocimiento, const string& pregunta) {
    // Primero intenta búsqueda exacta  
    string respuestaExacta = buscarExacto(conocimiento, pregunta);
    if (!respuestaExacta.empty()) {
        return respuestaExacta;
    }

    // Luego intenta por palabras clave con umbral
    string respuestaPalabras = buscarPorPalabrasClave(conocimiento, pregunta);
    if (!respuestaPalabras.empty()) {
        return respuestaPalabras;
    }

    // Si no encuentra nada, retorna mensaje estándar
    return "No se encontró una respuesta precisa en la base de datos. Por favor, verifica la ortografía o intenta con otros términos.";
}

// Busca coincidencias en SQL (modificado para mejor mensaje)
void buscarCoincidencias(const char* archivo, const char* palabraClave) {
    ifstream file(archivo);
    if (!file.is_open()) {
        cerr << "Error abriendo el archivo " << archivo << endl;
        return;
    }

    string linea;
    int numeroLinea = 1;
    bool encontrado = false;
    string palabraLower = palabraClave;
    transform(palabraLower.begin(), palabraLower.end(), palabraLower.begin(), ::tolower);

    while (getline(file, linea)) {
        string lineaLower = linea;
        transform(lineaLower.begin(), lineaLower.end(), lineaLower.begin(), ::tolower);

        if (lineaLower.find(palabraLower) != string::npos) {
            cout << "Línea " << numeroLinea << ": " << linea << endl;
            encontrado = true;
        }
        numeroLinea++;
    }

    if (!encontrado) {
        cout << "No se encontraron coincidencias para '" << palabraClave
            << "' en la base de datos SQL." << endl;
    }

    file.close();
}

//Estructura para almacenar respuestas en API de ChatGPT
struct RespuestaAPI {
    char* datos;
    size_t tamano;
};

//Callback de escritura de respuesta de la API
static size_t escribirRespuesta(void* contenido, size_t size, size_t nmemb, void* userp) {
    size_t total = size * nmemb;
    RespuestaAPI* resp = static_cast<RespuestaAPI*>(userp);

    char* ptr = reinterpret_cast<char*>(realloc(resp->datos, resp->tamano + total + 1));
    if (!ptr) return 0;

    resp->datos = ptr;
    memcpy(&(resp->datos[resp->tamano]), contenido, total);
    resp->tamano += total;
    resp->datos[resp->tamano] = '\0';
    return total;
}

//Busca una respuesta en la base de datos de SQL
string buscarEnBD(const string& pregunta) {
    String^ cadenaConexion = "Data Source=VICTUS;Initial Catalog=Chatdatabase;User ID=sa;Password=Cacadeperico123!;";
    SqlConnection^ conexion = gcnew SqlConnection(cadenaConexion);
    string respuesta = "";

    try {
        conexion->Open();
        String^ consulta = "SELECT Respuesta FROM Conocimiento WHERE Pregunta = @Pregunta";
        SqlCommand^ comando = gcnew SqlCommand(consulta, conexion);
        comando->Parameters->AddWithValue("@Pregunta", marshal_as<String^>(normalizarTexto(pregunta)));

        SqlDataReader^ lector = comando->ExecuteReader();
        if (lector->Read()) {
            respuesta = marshal_as<string>(lector["Respuesta"]->ToString());
        }
        conexion->Close();
    }
    catch (Exception^ ex) {
        Console::WriteLine("Error al buscar en BD: {0}", ex->Message);
    }

    return respuesta;
}

//Escapar caracteres especiales en .JSON
string escape_json(const string& s) {
    ostringstream o;
    for (auto c = s.cbegin(); c != s.cend(); c++) {
        switch (*c) {
        case '"': o << "\\\""; break;
        case '\\': o << "\\\\"; break;
        case '\b': o << "\\b"; break;
        case '\f': o << "\\f"; break;
        case '\n': o << "\\n"; break;
        case '\r': o << "\\r"; break;
        case '\t': o << "\\t"; break;
        default:
            if ('\x00' <= *c && *c <= '\x1f') {
                o << "\\u" << hex << setw(4) << setfill('0') << (int)*c;
            }
            else {
                o << *c;
            }
        }
    }
    return o.str();
}

//Función para consultar a la API de OpenAI
string consultarOpenAI(const string& pregunta) {
    CURL* curl;
    CURLcode res;
    string readBuffer;

    const string api_key = "Bearer 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";
    if (api_key == "Bearer error") {
        return "ERROR: Por favor configura tu API key de OpenAI en el código (reemplaza en const string)";
    }

    const string url = "https://api.openai.com/v1/chat/completions";

    // Crear el JSON de la solicitud
    json requestData = {
        {"model", "gpt-3.5-turbo"},
        {"messages", {{{"role", "user"}, {"content", pregunta}}}}
    };
    string jsonData = requestData.dump();

    curl_global_init(CURL_GLOBAL_ALL);
    curl = curl_easy_init();

    if (curl) {
        struct curl_slist* headers = nullptr;
        headers = curl_slist_append(headers, "Content-Type: application/json");
        headers = curl_slist_append(headers, ("Authorization: " + api_key).c_str());

        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, jsonData.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
        curl_easy_setopt(curl, CURLOPT_TIMEOUT, 15L);

        res = curl_easy_perform(curl);

        string resultado;
        if (res != CURLE_OK) {
            resultado = "Error al conectar con OpenAI: " + string(curl_easy_strerror(res));
        }
        else {
            try {
                json response = json::parse(readBuffer);
                if (response.contains("choices") && !response["choices"].empty()) {
                    resultado = response["choices"][0]["message"]["content"];
                }
                else {
                    resultado = "Error: No se pudo obtener respuesta de OpenAI";
                }
            }
            catch (...) {
                resultado = "Error procesando la respuesta de OpenAI";
            }
        }

        curl_easy_cleanup(curl);
        curl_slist_free_all(headers);
        curl_global_cleanup();

        return resultado;
    }
    return "Error al inicializar cURL";
}

// Funciones para administrar la base de datos
void mostrarBase() {
    SQLAllocHandle(SQL_HANDLE_STMT, hDbc, &hStmt);
    SQLExecDirectA(hStmt, (SQLCHAR*)"SELECT Pregunta, Respuesta FROM Conocimiento ORDER BY Pregunta", SQL_NTS);

    SQLCHAR pregunta[512], respuesta[1024];
    SQLLEN lenPregunta, lenRespuesta;

    cout << "\n=== BASE DE CONOCIMIENTO ===\n";
    while (SQLFetch(hStmt) == SQL_SUCCESS) {
        SQLGetData(hStmt, 1, SQL_C_CHAR, pregunta, sizeof(pregunta), &lenPregunta);
        SQLGetData(hStmt, 2, SQL_C_CHAR, respuesta, sizeof(respuesta), &lenRespuesta);
        cout << "Pregunta: " << pregunta << "\nRespuesta: " << respuesta << "\n---\n";
    }

    SQLFreeHandle(SQL_HANDLE_STMT, hStmt);
}

//Búsqueda de preguntas de SQL
void buscarPregunta() { 
    string keyword;
    cout << "Ingrese su pregunta (puedes ver las preguntas disponibles en la opcion 1): ";
    cin.ignore();
    getline(cin, keyword);

    SQLHSTMT hStmtLocal;
    SQLAllocHandle(SQL_HANDLE_STMT, hDbc, &hStmtLocal);

    string consulta = "SELECT Pregunta, Respuesta FROM Conocimiento WHERE Pregunta LIKE ?";
    SQLPrepareA(hStmtLocal, (SQLCHAR*)consulta.c_str(), SQL_NTS);

    string likePattern = "%" + keyword + "%";
    SQLBindParameter(hStmtLocal, 1, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_VARCHAR,
        likePattern.size(), 0, (SQLCHAR*)likePattern.c_str(),
        likePattern.size(), NULL);

    SQLExecute(hStmtLocal);

    SQLCHAR pregunta[512], respuesta[1024];
    SQLLEN lenPregunta, lenRespuesta;
    bool encontrada = false;

    sleep_for(seconds(2));

    cout << "\n=== RESULTADOS DE BUSQUEDA ===\n"; //Procesa preguntas de SQL
    while (SQLFetch(hStmtLocal) == SQL_SUCCESS) {
        encontrada = true;
        SQLGetData(hStmtLocal, 1, SQL_C_CHAR, pregunta, sizeof(pregunta), &lenPregunta);
        SQLGetData(hStmtLocal, 2, SQL_C_CHAR, respuesta, sizeof(respuesta), &lenRespuesta);
        cout << "Procesando..." << endl;
        sleep_for(seconds(2));
        cout << "\r                \r";
        cout << "Pregunta: " << pregunta << "\nRespuesta: " << respuesta << "\n---\n";
    }

    if (!encontrada) {
        cout << "No se encontraron resultados, tal vez" << COLOR_RED << " no se encuentra la pregunta en base de datos" << COLOR_RESET << " o " << COLOR_RED << " no estas conectado al servidor de SQL" COLOR_RESET << ".\n";
    }
    SQLFreeHandle(SQL_HANDLE_STMT, hStmtLocal);
}

//Función para contar preguntas
void contarPreguntas() {
    SQLAllocHandle(SQL_HANDLE_STMT, hDbc, &hStmt);
    SQLExecDirectA(hStmt, (SQLCHAR*)"SELECT COUNT(*) FROM Conocimiento", SQL_NTS);

    SQLINTEGER count;
    SQLBindCol(hStmt, 1, SQL_C_SLONG, &count, 0, NULL);
    SQLFetch(hStmt);
    cout << "Procesando..." << endl;
    sleep_for(seconds(2));
    cout << "\r             \r" << endl;
    cout << "Total de preguntas registradas: " << COLOR_YELLOW << count << COLOR_RESET << endl;

    SQLFreeHandle(SQL_HANDLE_STMT, hStmt);
}

//Encabezado con colores
void mostrarEncabezado() {
    limpiarPantalla();
    cout << COLOR_CYAN;
    mostrarLinea();
    cout << "  ____ _           _   ____        _   \n";
    cout << " / ___| |         | | | __ )  ___ | |_ \n";
    cout << "| |   | |__   __ _| |_|  _ \\ / _ \\| __|\n";
    cout << "| |___| '_ \\ / _` | __| |_) | (_) | |_ \n";
    cout << " \\____|_| |_|\\__,_|\\__|____/ \\___/ \\__|\n";
    cout << COLOR_RESET;
    mostrarLinea();
}

// Menú principal
void mostrarMenu() {
    mostrarEncabezado();
    cout << COLOR_YELLOW << "MENU INTERACTIVO DE BASE DE DATOS DE .txt y SQL\n" << COLOR_RESET << endl;
    mostrarLinea();
    
    cout << COLOR_CYAN << "Bienvenido a este ChatBOT que se trata de Compatibilidad de procesadores en PC\n" << COLOR_RESET << endl;
    cout << "\n";
    cout << COLOR_GREEN << "Entendimiento de busqueda:\n" << COLOR_RESET;
    cout << "-Puedes preguntar por cualquier pregunta relacionada a el tema, y el ChatBOT buscara la respuesta en la base de datos vinculada de SQL SMS 20\n";
    cout << "-Tambien puedes acceder a nuestro ChatBot local con base de datos en un archivo .txt accediendo a la opcion 4.\n";
    cout << "\n";
    cout << COLOR_RED << "Disfruta del ChatBOT de SQL.\n" << COLOR_RESET;
    cout << "\n";
    cout << "1. Ver todas las preguntas\n";
    cout << "2. Preguntar\n";
    cout << "3. Contar preguntas registradas\n";
    cout << "4. Modo ChatBot\n";
    cout << "0. Salir\n";
    mostrarLinea();
    cout << "Seleccione una opcion: ";
}

// Modo ChatBot
void modoChatBot() {
    limpiarPantalla();
    cout << COLOR_MAGENTA << "=====Modo Chatbot Local=====\n" << COLOR_RESET << endl;
    cout << "\n";
    cout << COLOR_GREEN << "Entendimiento de busqueda:\n" << COLOR_RESET;
    cout << "1. Primero busca en conocimiento local (archivo .txt)" << endl;
    cout << "2. Si la pregunta no aparece en .txt, entonces se busca en la base de datos de SQL." << endl;
    cout << "3. Finalmente ofrece consultar a OpenAI si no encuentra respuesta en los archivos locales." << endl;
    cout << "\n";
    cout << COLOR_RED << "Opciones:\n" << COLOR_RESET;
    cout << " - '[pregunta]': Busca la respuesta en las bases de datos del archivo conocimientos.txt" << endl;
    cout << " - 'openai [pregunta]': Consulta a OpenAI" << endl;
    cout << " - 'menu': Volver al menu principal" << endl << endl;

    //Manejo del chatbot local
    string input;
    while (true) {
        cout << "\nTu: ";
        getline(cin, input);

        if (input == "menu") {
            break;
        }

        if (input == "adios") {
            cout << "Bot: Hasta luego!" << endl;
            exit(0);
        }

        if (input.rfind("openai ", 0) == 0) {
            string pregunta = input.substr(7);
            cout << "Consultando una respuesta corta a " << COLOR_GREEN << "OpenAI...\n" << COLOR_RESET;
            string respuesta = consultarOpenAI(pregunta);

            this_thread::sleep_for(chrono::seconds(2));
            cout << COLOR_GREEN << "Bot (OpenAI): " << COLOR_RESET << respuesta << endl;
            continue;
        }

        // Flujo principal de búsqueda de respuestas
        string respuesta;
        cout << "Bot: ";
        this_thread::sleep_for(chrono::seconds(2));  // Delay común para todas las respuestas

        // 1. Buscar en archivo local
        respuesta = buscarExacto(conocimiento, input);

        // 2. Si no está en archivo, buscar en BD
        if (respuesta.empty()) {
            respuesta = buscarEnBD(input);
            if (!respuesta.empty()) {
                conocimiento[input] = respuesta;  // Cachear en memoria
            }
        }

        // 3. Si no está en BD, buscar por palabras clave
        if (respuesta.empty()) {
            respuesta = buscarPorPalabrasClave(conocimiento, input);
        }

        // Mostrar la respuesta encontrada o mensaje de error
        if (!respuesta.empty()) {
            cout << respuesta << endl;
        }
        else {
            cout << COLOR_RED << "No se encontro una respuesta en la base de datos Conocimiento.txt." << COLOR_RESET << " Trata de buscar la respuesta en la " << COLOR_YELLOW << "base de datos de SQL," << COLOR_RESET << " o si prefieres, buscalo en " << COLOR_GREEN << "openai[pregunta].\n" <<  COLOR_RESET;
        }
    }
}

//Programa principal
int main() {
    setlocale(LC_ALL, "es_ES.UTF-8");
    SetConsoleCP(CP_UTF8);
    SetConsoleOutputCP(CP_UTF8);

    //Error en la conexión a la base de datos
    if (!conectarBD()) {
        cout << "Error al conectar con la base de datos.\n";
        return 1;
    }

    // Cargar conocimiento desde archivo y base de datos
    cargarConocimiento(conocimiento, "conocimiento.txt");
    cargarConocimientoDesdeBD(conocimiento);

    //Case y switch para manejo de interfaz dinámica
    int opcion;
    do {
        limpiarPantalla();
        mostrarMenu();
        cin >> opcion;
        cin.ignore(); // Limpiar el buffer de entrada

        switch (opcion) {
        case 1: //Muestra preguntas de SQL
            limpiarPantalla();
            mostrarBase();
            pausar();
            break;
        case 2: //Buscar preguntas
            limpiarPantalla();
            buscarPregunta();
            pausar();
            break;
        case 3: //Contar preguntas de SQL
            limpiarPantalla();
            contarPreguntas();
            pausar();
            break;
        case 4: //Búsqueda de archivos locales
            modoChatBot();
            break;
        case 0: //Salir del programa
            cout << "Saliendo...\n";
            break;
        default: //Opción no válida
            cout << "Opción no válida.\n";
            pausar();
        }
    } while (opcion != 0);

    desconectarBD();
    return 0;
}
