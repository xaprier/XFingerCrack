#include "WordGenerator.hpp"

#include <QDebug>
#include <QDirIterator>
#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <chrono>

WordGenerator::WordGenerator(QObject* parent)
    : QObject(parent),
      m_randomEngine(std::chrono::system_clock::now().time_since_epoch().count()) {}

bool WordGenerator::LoadAllWordlists() {
    QDirIterator it(":/wordlist", QStringList() << "*.json", QDir::Files);
    bool loaded = false;

    while (it.hasNext()) {
        QString filePath = it.next();
        if (LoadWordlistFromJson(filePath)) {
            loaded = true;
        }
    }

    // Set default language to english if available, otherwise first available
    if (!m_wordlists.isEmpty()) {
        if (m_wordlists.contains("english")) {
            LoadWordlist("english");
        } else {
            LoadWordlist(m_wordlists.firstKey());
        }
    }

    return loaded;
}

bool WordGenerator::LoadWordlistFromJson(const QString& filePath) {
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qWarning() << "Failed to open wordlist file:" << filePath;
        return false;
    }

    QByteArray data = file.readAll();
    file.close();

    QJsonDocument doc = QJsonDocument::fromJson(data);
    if (!doc.isObject()) {
        qWarning() << "Invalid JSON format in:" << filePath;
        return false;
    }

    QJsonObject obj = doc.object();
    WordList wordlist;
    wordlist.name = obj["name"].toString();
    wordlist.bcp47 = obj["bcp47"].toString();

    QJsonArray wordsArray = obj["words"].toArray();
    for (const QJsonValue& value : wordsArray) {
        wordlist.words.append(value.toString());
    }

    if (wordlist.name.isEmpty() || wordlist.words.isEmpty()) {
        qWarning() << "Invalid wordlist data in:" << filePath;
        return false;
    }

    m_wordlists[wordlist.name] = wordlist;

    return true;
}

bool WordGenerator::LoadWordlist(const QString& languageName) {
    if (!m_wordlists.contains(languageName)) {
        qWarning() << "Language not found:" << languageName;
        qWarning() << "Available languages:" << m_wordlists.keys();
        return false;
    }

    m_currentLanguage = languageName;
    m_currentWords = m_wordlists[languageName].words;

    return true;
}

QStringList WordGenerator::AvailableLanguages() const {
    return m_wordlists.keys();
}

QStringList WordGenerator::GenerateWords(int count) {
    if (m_currentWords.isEmpty()) {
        qWarning() << "No words loaded";
        return QStringList();
    }

    QStringList result;
    std::uniform_int_distribution<int> distribution(0, m_currentWords.size() - 1);

    for (int i = 0; i < count; ++i) {
        int index = distribution(m_randomEngine);
        result.append(m_currentWords[index]);
    }

    return result;
}

int WordGenerator::WordCount() const { return m_currentWords.size(); }

QString WordGenerator::CurrentLanguage() const { return m_currentLanguage; }
