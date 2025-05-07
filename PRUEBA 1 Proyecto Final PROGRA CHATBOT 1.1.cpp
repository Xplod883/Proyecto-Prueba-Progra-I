#define _CRT_SECURE_NO_WARNINGS

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
#include <msclr/marshal_cppstd.h>
#include "stdafx.h"

using namespace System;
using namespace System::Data;
using namespace System::Data::SqlClient;
using namespace msclr::interop;
using namespace std;

string trim(const string& str) {
    size_t start = str.find_first_not_of(" \t\n\r\f\v");
    size_t end = str.find_last_not_of(" \t\n\r\f\v");
    return (start == string::npos || end == string::npos) ? "" : str.substr(start, end - start + 1);
}

void cargarConocimiento(map<string, string>& conocimiento, const string& nombreArchivo) {
    ifstream archivo(nombreArchivo);
    if (!archivo.is_open()) {
        cout << "Error al abrir el archivo de conocimiento." << endl;
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

void cargarConocimientoDesdeBD(map<string, string>& conocimiento) {
    String^ cadenaConexion = "Data Source=VICTUS;Initial Catalog=ChatbotDB;User ID=sa;Password=Cacadeperico123!;";
    SqlConnection^ conexion = gcnew SqlConnection(cadenaConexion);

    cout << "Se cargaron " << conocimiento.size() << " preguntas desde la BD" << endl;

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

string buscarExacto(const map<string, string>& conocimiento, const string& pregunta) {
    string preguntaLower = pregunta;
    transform(preguntaLower.begin(), preguntaLower.end(), preguntaLower.begin(), ::tolower);

    for (const auto& par : conocimiento) {
        string claveLower = par.first;
        transform(claveLower.begin(), claveLower.end(), claveLower.begin(), ::tolower);

        if (preguntaLower == claveLower) {
            return par.second;
        }
    }
    return "";
}

vector<string> dividirPalabras(const string& frase) {
    vector<string> palabras;
    stringstream ss(frase);
    string palabra;
    while (ss >> palabra) {
        transform(palabra.begin(), palabra.end(), palabra.begin(), ::tolower);
        palabras.push_back(palabra);
    }
    return palabras;
}

string buscarPorPalabrasClave(const map<string, string>& conocimiento, const string& pregunta) {
    vector<string> palabrasPregunta = dividirPalabras(pregunta);
    string mejorRespuesta = "";
    int maxCoincidencias = 0;

    for (const auto& par : conocimiento) {
        vector<string> palabrasBase = dividirPalabras(par.first);

        int coincidencias = 0;
        for (const string& palabraPregunta : palabrasPregunta) {
            for (const string& palabraBase : palabrasBase) {
                if (palabraPregunta == palabraBase) {
                    coincidencias++;
                }
            }
        }

        if (coincidencias > maxCoincidencias) {
            maxCoincidencias = coincidencias;
            mejorRespuesta = par.second;
        }
    }
    return (maxCoincidencias > 0) ? mejorRespuesta : "";
}

void buscarCoincidencias(const char* archivo, const char* palabraClave) {
    ifstream file(archivo);
    if (!file.is_open()) {
        cerr << "Error abriendo el archivo " << archivo << endl;
        return;
    }

    string linea;
    int numeroLinea = 1;
    bool encontrado = false;

    while (getline(file, linea)) {
        if (linea.find(palabraClave) != string::npos) {
            cout << "Línea " << numeroLinea << ": " << linea << endl;
            encontrado = true;
        }
        numeroLinea++;
    }

    if (!encontrado) {
        cout << "No se encontraron coincidencias para '" << palabraClave << "'" << endl;
    }

    file.close();
}

struct RespuestaAPI {
    char* datos;
    size_t tamano;
};

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

string buscarEnBD(const string& pregunta) {
    String^ cadenaConexion = "Data Source=VICTUS;Initial Catalog=Chatdatabase;User ID=sa;Password=Cacadeperico123!;";
    SqlConnection^ conexion = gcnew SqlConnection(cadenaConexion);
    string respuesta = "";

    try {
        conexion->Open();
        String^ consulta = "SELECT Respuesta FROM Conocimiento WHERE Pregunta = @Pregunta";
        SqlCommand^ comando = gcnew SqlCommand(consulta, conexion);
        comando->Parameters->AddWithValue("@Pregunta", marshal_as<String^>(pregunta));

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

string consultarOpenAI(const string& pregunta) {
    CURL* curl;
    CURLcode res;
    RespuestaAPI respuesta = { nullptr, 0 };

    const string api_key = "Bearer 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";
    if (api_key == "Bearer error") {
        return "ERROR: Por favor configura tu API key de OpenAI en el código (reemplaza en const string)";
    }

    const string url = "https://api.openai.com/v1/chat/completions";
    string json = "{"
        "\"model\": \"gpt-3.5-turbo\","
        "\"messages\": [{\"role\": \"user\", \"content\": \"" + escape_json(pregunta) + "\"}],"
        "\"temperature\": 0.7"
        "}";

    curl_global_init(CURL_GLOBAL_ALL);
    curl = curl_easy_init();

    if (curl) {
        struct curl_slist* headers = nullptr;
        headers = curl_slist_append(headers, "Content-Type: application/json");
        headers = curl_slist_append(headers, ("Authorization: " + api_key).c_str());

        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, json.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, escribirRespuesta);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, static_cast<void*>(&respuesta));
        curl_easy_setopt(curl, CURLOPT_TIMEOUT, 15L);
        cout << "\nJSON enviado:\n" << json << endl;
        res = curl_easy_perform(curl);

        string resultado;
        if (res != CURLE_OK) {
            resultado = "Error al conectar con OpenAI: " + string(curl_easy_strerror(res));
        }
        else if (respuesta.datos) {
            string respuesta_json = respuesta.datos;

            size_t contenido_pos = respuesta_json.find("\"content\":\"");
            if (contenido_pos != string::npos) {
                size_t inicio = contenido_pos + 10;
                size_t fin = respuesta_json.find("\"", inicio);
                if (fin != string::npos) {
                    resultado = respuesta_json.substr(inicio, fin - inicio);
                    size_t pos = 0;
                    while ((pos = resultado.find("\\n", pos)) != string::npos) {
                        resultado.replace(pos, 2, "\n");
                        pos += 1;
                    }
                    pos = 0;
                    while ((pos = resultado.find("\\\"", pos)) != string::npos) {
                        resultado.replace(pos, 2, "\"");
                        pos += 1;
                    }
                }
                else {
                    resultado = "Error: No se pudo encontrar el fin de la respuesta en el JSON";
                }
            }
            else {
                resultado = "Respuesta completa de OpenAI:\n" + respuesta_json;
            }
        }

        curl_easy_cleanup(curl);
        curl_slist_free_all(headers);
        free(respuesta.datos);
        curl_global_cleanup();

        return resultado;
    }
    return "Error al inicializar cURL";
}

void agregarConocimientoABD(const string& pregunta, const string& respuesta) {
    String^ cadenaConexion = "Data Source=VICTUS;Initial Catalog=Chatdatabase;User ID=sa;Password=Cacadeperico123!;";
    SqlConnection^ conexion = gcnew SqlConnection(cadenaConexion);

    try {
        conexion->Open();
        String^ consulta = "INSERT INTO Conocimiento (Pregunta, Respuesta) VALUES (@Pregunta, @Respuesta)";
        SqlCommand^ comando = gcnew SqlCommand(consulta, conexion);
        comando->Parameters->AddWithValue("@Pregunta", marshal_as<String^>(pregunta));
        comando->Parameters->AddWithValue("@Respuesta", marshal_as<String^>(respuesta));
        comando->ExecuteNonQuery();
        conexion->Close();
    }
    catch (Exception^ ex) {
        Console::WriteLine("Error al agregar a BD: {0}", ex->Message);
    }
}

void agregarConocimientoAArchivo(const string& pregunta, const string& respuesta, const string& archivo) {
    ofstream file(archivo, ios::app);
    if (file.is_open()) {
        file << pregunta << "|" << respuesta << endl;
        file.close();
    }
    else {
        cerr << "Error al abrir el archivo para agregar conocimiento" << endl;
    }
}

int main() {
    setlocale(LC_ALL, "es_ES.UTF-8");

    map<string, string> conocimiento;

    // Cargar conocimiento desde archivo y base de datos
    cargarConocimiento(conocimiento, "conocimiento.txt");
    cargarConocimientoDesdeBD(conocimiento);

    cout << "=== Chatbot Inteligente ===" << endl;
    cout << "Flujo de búsqueda:" << endl;
    cout << "1. Primero busca en conocimiento local (archivo)" << endl;
    cout << "2. Luego en la base de datos" << endl;
    cout << "3. Finalmente ofrece consultar a OpenAI si no encuentra respuesta" << endl;
    cout << "\nOpciones especiales:" << endl;
    cout << " - 'buscar [palabra]': Busca en archivos" << endl;
    cout << " - 'openai [pregunta]': Consulta a OpenAI (extra)" << endl;
    cout << " - 'agregar [pregunta]|[respuesta]': Añade nuevo conocimiento" << endl;
    cout << " - 'adios': Terminar el programa" << endl << endl;

    string input;
    while (true) {
        cout << "\nTu: ";
        getline(cin, input);

        if (input == "adios") {
            cout << "Bot: Hasta luego!" << endl;
            break;
        }

        if (input.rfind("buscar ", 0) == 0) {
            string palabra = trim(input.substr(7));
            if (!palabra.empty()) {
                cout << "=== Resultados de búsqueda para '" << palabra << "' ===" << endl;
                buscarCoincidencias("conocimiento.txt", palabra.c_str());
            }
            else {
                cout << "Debes especificar una palabra para buscar" << endl;
            }
            continue;
        }

        if (input.rfind("openai ", 0) == 0) {
            string pregunta = input.substr(7);
            cout << "Consultando a OpenAI...\n";
            string respuesta = consultarOpenAI(pregunta);
            cout << "Bot (OpenAI): " << respuesta << endl;

            // Preguntar si quiere guardar la respuesta
            cout << "\n¿Deseas guardar esta respuesta en la base de conocimiento? (s/n): ";
            getline(cin, input);
            if (input == "s" || input == "S") {
                agregarConocimientoABD(pregunta, respuesta);
                agregarConocimientoAArchivo(pregunta, respuesta, "conocimiento.txt");
                conocimiento[pregunta] = respuesta;
                cout << "Respuesta guardada en la base de conocimiento." << endl;
            }
            continue;
        }

        if (input.rfind("agregar ", 0) == 0) {
            string datos = input.substr(8);
            size_t separador = datos.find("|");
            if (separador != string::npos) {
                string pregunta = trim(datos.substr(0, separador));
                string respuesta = trim(datos.substr(separador + 1));

                agregarConocimientoABD(pregunta, respuesta);
                agregarConocimientoAArchivo(pregunta, respuesta, "conocimiento.txt");
                conocimiento[pregunta] = respuesta;

                cout << "Bot: Nuevo conocimiento agregado correctamente." << endl;
            }
            else {
                cout << "Bot: Formato incorrecto. Usa: agregar [pregunta]|[respuesta]" << endl;
            }
            continue;
        }

        // Flujo principal de búsqueda de respuestas
        string respuesta = buscarExacto(conocimiento, input); // Primero busca en el mapa (archivo)

        if (respuesta.empty()) {
            // Si no encontró en archivo, buscar en BD directamente
            respuesta = buscarEnBD(input);
            if (!respuesta.empty()) {
                // Si encontró en BD, agregar al conocimiento en memoria para futuras consultas
                conocimiento[input] = respuesta;
                cout << "Bot (desde BD): " << respuesta << endl;
            }
        }

        if (respuesta.empty()) {
            // Si no encontró coincidencia exacta, intentar con palabras clave
            respuesta = buscarPorPalabrasClave(conocimiento, input);
            if (!respuesta.empty()) {
                cout << "Bot (coincidencia parcial): " << respuesta << endl;
            }
        }

        if (respuesta.empty()) {
            // Si no encontró en ningún lado
            respuesta = "No tengo una respuesta para eso. ¿Quieres que consulte a OpenAI? (escribe 'openai [tu pregunta]')";
            cout << "Bot: " << respuesta << endl;
        }
        else if (respuesta != "") {
            cout << "Bot: " << respuesta << endl;
        }
    }

    return 0;
}