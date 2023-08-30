#include <string>
#include <unordered_map>
#include <vector>
#include <algorithm>

class VersionControl {
	struct UpdateInfo {
		std::string category;
		std::string date;
		std::string description;
	};

	std::vector<UpdateInfo> updates;
	std::unordered_map<std::string, bool> categoryShared;
	std::string currentCategory;

	static std::string createVersion(int sharedVer, int uniqueVer) {
		return std::string("v0.") + std::to_string(sharedVer) + "." + std::to_string(uniqueVer);
	}

public:
	std::string getVersion() const {
		int sharedVer = 0, uniqueVer = 0;
		for (auto& update : updates) {
			if (categoryShared.at(update.category)) sharedVer++;
			else uniqueVer++;
		}
		return createVersion(sharedVer, uniqueVer);
	}

	void setSharedCategory(std::string name) {
		currentCategory = name;
		categoryShared[name] = true;
	}

	void setUniqueCategory(std::string name) {
		currentCategory = name;
		categoryShared[name] = false;
	}

	void addUpdate(std::string date, std::string description) {
		UpdateInfo e;
		e.category = currentCategory;
		e.date = date;
		e.description = description;
		updates.push_back(e);
	}

	std::string getHistory() {
		int sharedVer = 0, uniqueVer = 0;
		std::vector<std::string> lines;
		std::string result;
		std::sort(updates.begin(), updates.end(), [](const UpdateInfo& a, const UpdateInfo& b)->bool {return a.date.compare(b.date) < 0;});
		for (auto& update : updates) {
			if (categoryShared.at(update.category)) sharedVer++;
			else uniqueVer++;
			lines.push_back(createVersion(sharedVer, uniqueVer) + " " + update.date + ": " + update.category + " - " + update.description + "\n");
		}
		std::reverse(lines.begin(), lines.end());
		for (auto& line : lines) result += line;
		return result;
	}

} VERSION_CONTROL;