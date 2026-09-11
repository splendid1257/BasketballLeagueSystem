#include "storage/DataStore.h"

#include <QCoreApplication>
#include <QCryptographicHash>
#include <QDir>
#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>

#include <algorithm>

namespace {

QJsonObject statsToJson(const PlayerStats &s)
{
    QJsonObject o;
    o["playerId"] = s.playerId;
    o["playerName"] = s.playerName;
    o["threePointers"] = s.threePointers;
    o["rebounds"] = s.rebounds;
    o["dunks"] = s.dunks;
    o["steals"] = s.steals;
    return o;
}

PlayerStats statsFromJson(const QJsonObject &o)
{
    PlayerStats s;
    s.playerId = o["playerId"].toString();
    s.playerName = o["playerName"].toString();
    s.threePointers = o["threePointers"].toInt();
    s.rebounds = o["rebounds"].toInt();
    s.dunks = o["dunks"].toInt();
    s.steals = o["steals"].toInt();
    return s;
}

QJsonArray statsArrayToJson(const QVector<PlayerStats> &list)
{
    QJsonArray arr;
    for (const PlayerStats &s : list)
        arr.append(statsToJson(s));
    return arr;
}

QVector<PlayerStats> statsArrayFromJson(const QJsonArray &arr)
{
    QVector<PlayerStats> list;
    for (const QJsonValue &v : arr)
        list.append(statsFromJson(v.toObject()));
    return list;
}

bool writeJsonArray(const QString &path, const QJsonArray &arr)
{
    QFile f(path);
    if (!f.open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Text))
        return false;
    f.write(QJsonDocument(arr).toJson(QJsonDocument::Indented));
    f.close();
    return true;
}

QJsonArray readJsonArray(const QString &path)
{
    QFile f(path);
    if (!f.open(QIODevice::ReadOnly | QIODevice::Text))
        return {};
    const QJsonDocument doc = QJsonDocument::fromJson(f.readAll());
    f.close();
    return doc.isArray() ? doc.array() : QJsonArray();
}

PlayerStats mk(const QString &id, const QString &name, int tp, int rb, int dk, int st)
{
    PlayerStats s;
    s.playerId = id;
    s.playerName = name;
    s.threePointers = tp;
    s.rebounds = rb;
    s.dunks = dk;
    s.steals = st;
    return s;
}

}  // namespace

DataStore::DataStore(QObject *parent)
    : QObject(parent)
{
    ensureDataDir();
}

void DataStore::ensureDataDir()
{
    m_dataDir = QCoreApplication::applicationDirPath() + QStringLiteral("/data");
    QDir().mkpath(m_dataDir);
    m_playersFile = m_dataDir + QStringLiteral("/players.json");
    m_matchesFile = m_dataDir + QStringLiteral("/matches.json");
    m_usersFile = m_dataDir + QStringLiteral("/users.json");
}

bool DataStore::load()
{
    ensureDataDir();
    const bool fresh = !QFile::exists(m_playersFile) && !QFile::exists(m_matchesFile);

    loadPlayers();
    loadMatches();
    loadUsers();

    if (fresh && m_matches.isEmpty()) {
        seedDemoData();
        save();
    }
    return true;
}

bool DataStore::save() const
{
    QJsonArray playersArr;
    for (const Player &p : m_players) {
        QJsonObject o;
        o["id"] = p.id;
        o["name"] = p.name;
        o["age"] = p.age;
        o["team"] = p.team;
        o["number"] = p.number;
        o["position"] = p.position;
        o["heightCm"] = p.heightCm;
        o["weightKg"] = p.weightKg;
        o["country"] = p.country;
        playersArr.append(o);
    }

    QJsonArray matchesArr;
    for (const Match &m : m_matches) {
        QJsonObject o;
        o["id"] = m.id;
        o["dateTime"] = m.dateTime.toString(Qt::ISODate);
        o["location"] = m.location;
        o["team1Name"] = m.team1Name;
        o["team2Name"] = m.team2Name;
        o["team1Players"] = statsArrayToJson(m.team1Players);
        o["team2Players"] = statsArrayToJson(m.team2Players);
        matchesArr.append(o);
    }

    QJsonArray usersArr;
    for (const User &u : m_users) {
        QJsonObject o;
        o["username"] = u.username;
        o["salt"] = u.salt;
        o["passwordHash"] = u.passwordHash;
        usersArr.append(o);
    }

    const bool ok = writeJsonArray(m_playersFile, playersArr)
                    && writeJsonArray(m_matchesFile, matchesArr)
                    && writeJsonArray(m_usersFile, usersArr);
    return ok;
}

bool DataStore::loadPlayers()
{
    m_players.clear();
    for (const QJsonValue &v : readJsonArray(m_playersFile)) {
        const QJsonObject o = v.toObject();
        Player p;
        p.id = o["id"].toString();
        p.name = o["name"].toString();
        p.age = o["age"].toInt();
        p.team = o["team"].toString();
        p.number = o["number"].toInt();
        p.position = o["position"].toString();
        p.heightCm = o["heightCm"].toInt();
        p.weightKg = o["weightKg"].toInt();
        p.country = o["country"].toString();
        if (p.isValid())
            m_players.append(p);
    }
    return true;
}

bool DataStore::loadMatches()
{
    m_matches.clear();
    for (const QJsonValue &v : readJsonArray(m_matchesFile)) {
        const QJsonObject o = v.toObject();
        Match m;
        m.id = o["id"].toString();
        m.dateTime = QDateTime::fromString(o["dateTime"].toString(), Qt::ISODate);
        m.location = o["location"].toString();
        m.team1Name = o["team1Name"].toString();
        m.team2Name = o["team2Name"].toString();
        m.team1Players = statsArrayFromJson(o["team1Players"].toArray());
        m.team2Players = statsArrayFromJson(o["team2Players"].toArray());
        if (m.isValid())
            m_matches.append(m);
    }
    return true;
}

bool DataStore::loadUsers()
{
    m_users.clear();
    for (const QJsonValue &v : readJsonArray(m_usersFile)) {
        const QJsonObject o = v.toObject();
        User u;
        u.username = o["username"].toString();
        u.salt = o["salt"].toString();
        u.passwordHash = o["passwordHash"].toString();
        if (!u.username.isEmpty())
            m_users.append(u);
    }
    return true;
}

// ---------------------------------------------------------------- players

bool DataStore::playerExists(const QString &id) const
{
    return std::any_of(m_players.cbegin(), m_players.cend(),
                       [&id](const Player &p) { return p.id == id; });
}

bool DataStore::addPlayer(const Player &p)
{
    if (!p.isValid() || playerExists(p.id))
        return false;
    m_players.append(p);
    save();
    emit changed();
    return true;
}

bool DataStore::updatePlayer(const QString &oldId, const Player &p)
{
    for (Player &existing : m_players) {
        if (existing.id == oldId) {
            // 若改了编号，检查新编号是否与他人冲突
            if (p.id != oldId && playerExists(p.id))
                return false;
            existing = p;
            // 同步更新所有场次中的冗余姓名/编号
            for (Match &m : m_matches) {
                for (PlayerStats &s : m.team1Players) {
                    if (s.playerId == oldId) { s.playerId = p.id; s.playerName = p.name; }
                }
                for (PlayerStats &s : m.team2Players) {
                    if (s.playerId == oldId) { s.playerId = p.id; s.playerName = p.name; }
                }
            }
            save();
            emit changed();
            return true;
        }
    }
    return false;
}

bool DataStore::removePlayer(const QString &id)
{
    const auto it = std::find_if(m_players.begin(), m_players.end(),
                                 [&id](const Player &p) { return p.id == id; });
    if (it == m_players.end())
        return false;
    m_players.erase(it);
    // 级联：从所有场次中移除该球员
    for (Match &m : m_matches) {
        m.team1Players.erase(std::remove_if(m.team1Players.begin(), m.team1Players.end(),
                                            [&id](const PlayerStats &s) { return s.playerId == id; }),
                             m.team1Players.end());
        m.team2Players.erase(std::remove_if(m.team2Players.begin(), m.team2Players.end(),
                                            [&id](const PlayerStats &s) { return s.playerId == id; }),
                             m.team2Players.end());
    }
    save();
    emit changed();
    return true;
}

Player DataStore::findPlayer(const QString &id) const
{
    for (const Player &p : m_players)
        if (p.id == id)
            return p;
    return {};
}

QStringList DataStore::teams() const
{
    QStringList list;
    for (const Player &p : m_players) {
        const QString t = p.team.trimmed();
        if (!t.isEmpty() && !list.contains(t))
            list.append(t);
    }
    list.sort();
    return list;
}

// ---------------------------------------------------------------- matches

bool DataStore::matchExists(const QString &id) const
{
    return std::any_of(m_matches.cbegin(), m_matches.cend(),
                       [&id](const Match &m) { return m.id == id; });
}

bool DataStore::addMatch(const Match &m)
{
    if (!m.isValid() || matchExists(m.id))
        return false;
    m_matches.append(m);
    save();
    emit changed();
    return true;
}

bool DataStore::updateMatch(const QString &oldId, const Match &m)
{
    for (Match &existing : m_matches) {
        if (existing.id == oldId) {
            if (m.id != oldId && matchExists(m.id))
                return false;
            existing = m;
            save();
            emit changed();
            return true;
        }
    }
    return false;
}

bool DataStore::removeMatch(const QString &id)
{
    const auto it = std::find_if(m_matches.begin(), m_matches.end(),
                                 [&id](const Match &m) { return m.id == id; });
    if (it == m_matches.end())
        return false;
    m_matches.erase(it);
    save();
    emit changed();
    return true;
}

Match DataStore::findMatch(const QString &id) const
{
    for (const Match &m : m_matches)
        if (m.id == id)
            return m;
    return {};
}

bool DataStore::addPlayerToMatch(const QString &matchId, int teamNo, const PlayerStats &s)
{
    for (Match &m : m_matches) {
        if (m.id != matchId)
            continue;
        QVector<PlayerStats> &list = (teamNo == 1) ? m.team1Players : m.team2Players;
        const bool dup = std::any_of(list.cbegin(), list.cend(),
                                     [&s](const PlayerStats &x) { return x.playerId == s.playerId; });
        if (dup)
            return false;
        list.append(s);
        save();
        emit changed();
        return true;
    }
    return false;
}

bool DataStore::removePlayerFromMatch(const QString &matchId, int teamNo, const QString &playerId)
{
    for (Match &m : m_matches) {
        if (m.id != matchId)
            continue;
        QVector<PlayerStats> &list = (teamNo == 1) ? m.team1Players : m.team2Players;
        const auto before = list.size();
        list.erase(std::remove_if(list.begin(), list.end(),
                                  [&playerId](const PlayerStats &x) { return x.playerId == playerId; }),
                   list.end());
        if (list.size() == before)
            return false;
        save();
        emit changed();
        return true;
    }
    return false;
}

bool DataStore::updatePlayerStats(const QString &matchId, int teamNo, const QString &playerId,
                                  const PlayerStats &s)
{
    for (Match &m : m_matches) {
        if (m.id != matchId)
            continue;
        QVector<PlayerStats> &list = (teamNo == 1) ? m.team1Players : m.team2Players;
        for (PlayerStats &existing : list) {
            if (existing.playerId == playerId) {
                existing = s;
                save();
                emit changed();
                return true;
            }
        }
        return false;
    }
    return false;
}

// ---------------------------------------------------------------- stats
PlayerStats DataStore::careerTotals(const QString &playerId) const
{
    PlayerStats total;
    const Player p = findPlayer(playerId);
    total.playerId = playerId;
    total.playerName = p.name;
    auto accumulate = [&total, &playerId](const QVector<PlayerStats> &list) {
        for (const PlayerStats &s : list) {
            if (s.playerId != playerId)
                continue;
            total.threePointers += s.threePointers;
            total.rebounds += s.rebounds;
            total.dunks += s.dunks;
            total.steals += s.steals;
        }
    };
    for (const Match &m : m_matches) {
        accumulate(m.team1Players);
        accumulate(m.team2Players);
    }
    return total;
}

QVector<QPair<QString, PlayerStats>> DataStore::playerMatchLog(const QString &playerId) const
{
    QVector<QPair<QString, PlayerStats>> log;
    for (const Match &m : m_matches) {
        for (const PlayerStats &s : m.team1Players)
            if (s.playerId == playerId)
                log.append({m.id, s});
        for (const PlayerStats &s : m.team2Players)
            if (s.playerId == playerId)
                log.append({m.id, s});
    }
    return log;
}

QVector<QPair<QString, int>> DataStore::leaderboard(Board board) const
{
    QVector<QPair<QString, int>> rows;
    for (const Player &p : m_players) {
        const PlayerStats t = careerTotals(p.id);
        int value = 0;
        switch (board) {
        case Board::Points: value = t.points(); break;
        case Board::ThreePointers: value = t.threePointers; break;
        case Board::Rebounds: value = t.rebounds; break;
        case Board::Dunks: value = t.dunks; break;
        case Board::Steals: value = t.steals; break;
        }
        rows.append({p.id, value});
    }
    std::sort(rows.begin(), rows.end(),
              [](const QPair<QString, int> &a, const QPair<QString, int> &b) {
                  if (a.second != b.second)
                      return a.second > b.second;
                  return a.first < b.first;
              });
    return rows;
}

int DataStore::totalPoints() const
{
    int total = 0;
    for (const Match &m : m_matches)
        total += m.team1Points() + m.team2Points();
    return total;
}

// ---------------------------------------------------------------- users

bool DataStore::userExists(const QString &username) const
{
    return std::any_of(m_users.cbegin(), m_users.cend(),
                       [&username](const User &u) { return u.username == username; });
}

void DataStore::addUser(const User &u)
{
    m_users.append(u);
    save();
}

User DataStore::findUser(const QString &username) const
{
    for (const User &u : m_users)
        if (u.username == username)
            return u;
    return {};
}

// ---------------------------------------------------------------- seed

void DataStore::seedDemoData()
{
    const Player players[] = {
        {"L001", QStringLiteral("勒布朗·詹姆斯"), 39, QStringLiteral("洛杉矶湖人"), 23, QStringLiteral("SF"), 206, 113, QStringLiteral("美国")},
        {"L002", QStringLiteral("安东尼·戴维斯"), 31, QStringLiteral("洛杉矶湖人"), 3, QStringLiteral("PF"), 208, 115, QStringLiteral("美国")},
        {"L003", QStringLiteral("奥斯汀·里夫斯"), 26, QStringLiteral("洛杉矶湖人"), 15, QStringLiteral("SG"), 196, 89, QStringLiteral("美国")},
        {"L004", QStringLiteral("八村塁"), 26, QStringLiteral("洛杉矶湖人"), 28, QStringLiteral("PF"), 203, 104, QStringLiteral("日本")},
        {"B001", QStringLiteral("杰森·塔图姆"), 26, QStringLiteral("波士顿凯尔特人"), 12, QStringLiteral("SF"), 203, 95, QStringLiteral("美国")},
        {"B002", QStringLiteral("杰伦·布朗"), 28, QStringLiteral("波士顿凯尔特人"), 7, QStringLiteral("SG"), 198, 101, QStringLiteral("美国")},
        {"B003", QStringLiteral("德里克·怀特"), 30, QStringLiteral("波士顿凯尔特人"), 9, QStringLiteral("PG"), 193, 86, QStringLiteral("美国")},
        {"B004", QStringLiteral("朱·霍勒迪"), 34, QStringLiteral("波士顿凯尔特人"), 4, QStringLiteral("PG"), 193, 93, QStringLiteral("美国")},
        {"G001", QStringLiteral("斯蒂芬·库里"), 36, QStringLiteral("金州勇士"), 30, QStringLiteral("PG"), 188, 84, QStringLiteral("美国")},
        {"G002", QStringLiteral("克莱·汤普森"), 34, QStringLiteral("金州勇士"), 11, QStringLiteral("SG"), 198, 98, QStringLiteral("美国")},
        {"G003", QStringLiteral("德雷蒙德·格林"), 34, QStringLiteral("金州勇士"), 23, QStringLiteral("PF"), 198, 104, QStringLiteral("美国")},
        {"G004", QStringLiteral("安德鲁·威金斯"), 29, QStringLiteral("金州勇士"), 22, QStringLiteral("SF"), 201, 89, QStringLiteral("加拿大")},
        {"C001", QStringLiteral("扎克·拉文"), 29, QStringLiteral("芝加哥公牛"), 8, QStringLiteral("SG"), 196, 91, QStringLiteral("美国")},
        {"C002", QStringLiteral("德玛尔·德罗赞"), 35, QStringLiteral("芝加哥公牛"), 11, QStringLiteral("SF"), 198, 100, QStringLiteral("美国")},
        {"C003", QStringLiteral("尼古拉·武切维奇"), 34, QStringLiteral("芝加哥公牛"), 9, QStringLiteral("C"), 211, 118, QStringLiteral("黑山")},
        {"C004", QStringLiteral("科比·怀特"), 24, QStringLiteral("芝加哥公牛"), 2, QStringLiteral("PG"), 193, 88, QStringLiteral("美国")},
    };
    for (const Player &p : players)
        m_players.append(p);

    auto makeMatch = [](const QString &id, const QString &dt, const QString &loc,
                        const QString &t1, const QString &t2,
                        const QVector<PlayerStats> &p1, const QVector<PlayerStats> &p2) {
        Match m;
        m.id = id;
        m.dateTime = QDateTime::fromString(dt, Qt::ISODate);
        m.location = loc;
        m.team1Name = t1;
        m.team2Name = t2;
        m.team1Players = p1;
        m.team2Players = p2;
        return m;
    };

    m_matches.append(makeMatch(
        "M001", "2026-01-05T19:30:00", QStringLiteral("斯台普斯中心"),
        QStringLiteral("洛杉矶湖人"), QStringLiteral("波士顿凯尔特人"),
        {mk("L001", QStringLiteral("勒布朗·詹姆斯"), 3, 8, 2, 1),
         mk("L002", QStringLiteral("安东尼·戴维斯"), 1, 12, 3, 2),
         mk("L003", QStringLiteral("奥斯汀·里夫斯"), 4, 4, 0, 1),
         mk("L004", QStringLiteral("八村塁"), 2, 6, 1, 0)},
        {mk("B001", QStringLiteral("杰森·塔图姆"), 5, 9, 1, 2),
         mk("B002", QStringLiteral("杰伦·布朗"), 2, 7, 2, 1),
         mk("B003", QStringLiteral("德里克·怀特"), 3, 4, 0, 3),
         mk("B004", QStringLiteral("朱·霍勒迪"), 1, 5, 0, 2)}));

    m_matches.append(makeMatch(
        "M002", "2026-01-08T20:00:00", QStringLiteral("大通中心"),
        QStringLiteral("金州勇士"), QStringLiteral("芝加哥公牛"),
        {mk("G001", QStringLiteral("斯蒂芬·库里"), 6, 5, 0, 2),
         mk("G002", QStringLiteral("克莱·汤普森"), 5, 3, 0, 1),
         mk("G003", QStringLiteral("德雷蒙德·格林"), 1, 9, 0, 2),
         mk("G004", QStringLiteral("安德鲁·威金斯"), 2, 6, 1, 1)},
        {mk("C001", QStringLiteral("扎克·拉文"), 3, 5, 2, 1),
         mk("C002", QStringLiteral("德玛尔·德罗赞"), 2, 6, 0, 1),
         mk("C003", QStringLiteral("尼古拉·武切维奇"), 1, 11, 2, 0),
         mk("C004", QStringLiteral("科比·怀特"), 4, 4, 0, 2)}));

    m_matches.append(makeMatch(
        "M003", "2026-01-12T19:00:00", QStringLiteral("TD花园"),
        QStringLiteral("波士顿凯尔特人"), QStringLiteral("金州勇士"),
        {mk("B001", QStringLiteral("杰森·塔图姆"), 4, 10, 2, 1),
         mk("B002", QStringLiteral("杰伦·布朗"), 3, 6, 1, 2),
         mk("B003", QStringLiteral("德里克·怀特"), 2, 5, 0, 2),
         mk("B004", QStringLiteral("朱·霍勒迪"), 1, 4, 0, 1)},
        {mk("G001", QStringLiteral("斯蒂芬·库里"), 7, 4, 0, 1),
         mk("G002", QStringLiteral("克莱·汤普森"), 4, 5, 0, 0),
         mk("G003", QStringLiteral("德雷蒙德·格林"), 0, 8, 0, 3),
         mk("G004", QStringLiteral("安德鲁·威金斯"), 1, 7, 2, 1)}));

    m_matches.append(makeMatch(
        "M004", "2026-01-15T20:30:00", QStringLiteral("联合中心"),
        QStringLiteral("芝加哥公牛"), QStringLiteral("洛杉矶湖人"),
        {mk("C001", QStringLiteral("扎克·拉文"), 2, 4, 3, 1),
         mk("C002", QStringLiteral("德玛尔·德罗赞"), 3, 5, 0, 2),
         mk("C003", QStringLiteral("尼古拉·武切维奇"), 1, 10, 2, 1),
         mk("C004", QStringLiteral("科比·怀特"), 3, 3, 0, 0)},
        {mk("L001", QStringLiteral("勒布朗·詹姆斯"), 2, 9, 3, 2),
         mk("L002", QStringLiteral("安东尼·戴维斯"), 0, 14, 4, 1),
         mk("L003", QStringLiteral("奥斯汀·里夫斯"), 3, 5, 0, 2),
         mk("L004", QStringLiteral("八村塁"), 1, 5, 0, 1)}));

    m_matches.append(makeMatch(
        "M005", "2026-01-19T19:30:00", QStringLiteral("大通中心"),
        QStringLiteral("金州勇士"), QStringLiteral("洛杉矶湖人"),
        {mk("G001", QStringLiteral("斯蒂芬·库里"), 5, 6, 0, 3),
         mk("G002", QStringLiteral("克莱·汤普森"), 6, 4, 0, 1),
         mk("G003", QStringLiteral("德雷蒙德·格林"), 1, 7, 0, 1),
         mk("G004", QStringLiteral("安德鲁·威金斯"), 2, 8, 1, 0)},
        {mk("L001", QStringLiteral("勒布朗·詹姆斯"), 4, 8, 2, 1),
         mk("L002", QStringLiteral("安东尼·戴维斯"), 1, 13, 2, 2),
         mk("L003", QStringLiteral("奥斯汀·里夫斯"), 2, 4, 0, 1),
         mk("L004", QStringLiteral("八村塁"), 3, 6, 1, 0)}));

    m_matches.append(makeMatch(
        "M006", "2026-02-02T20:00:00", QStringLiteral("TD花园"),
        QStringLiteral("波士顿凯尔特人"), QStringLiteral("芝加哥公牛"),
        {mk("B001", QStringLiteral("杰森·塔图姆"), 6, 11, 1, 2),
         mk("B002", QStringLiteral("杰伦·布朗"), 2, 5, 3, 1),
         mk("B003", QStringLiteral("德里克·怀特"), 4, 3, 0, 2),
         mk("B004", QStringLiteral("朱·霍勒迪"), 1, 6, 0, 3)},
        {mk("C001", QStringLiteral("扎克·拉文"), 1, 6, 1, 0),
         mk("C002", QStringLiteral("德玛尔·德罗赞"), 4, 7, 0, 1),
         mk("C003", QStringLiteral("尼古拉·武切维奇"), 2, 12, 1, 0),
         mk("C004", QStringLiteral("科比·怀特"), 2, 3, 0, 2)}));

    // 默认账号：admin / 123456（仅在尚无该账号时创建，避免重复）
    if (!userExists(QStringLiteral("admin"))) {
        User admin;
        admin.username = QStringLiteral("admin");
        admin.salt = QStringLiteral("d3f1a9c47b2e6085");
        {
            const QByteArray raw = (admin.salt + QStringLiteral("123456")).toUtf8();
            admin.passwordHash = QString::fromLatin1(
                QCryptographicHash::hash(raw, QCryptographicHash::Sha256).toHex());
        }
        m_users.append(admin);
    }
}
