#pragma once

#include "Domain/ClientVersion.h"
#include <cstdint>
#include <string>
#include <unordered_set>
#include <vector>

namespace MapEditor {
namespace Services {
class ClientVersionRegistry;
class ConfigService;
} // namespace Services

namespace UI {

class ClientPropertyEditor;

class ClientConfigurationDialog {
public:
  ClientConfigurationDialog();
  ~ClientConfigurationDialog();

  void open(Services::ClientVersionRegistry &registry,
            Services::ConfigService &config);
  bool render();
  void close();
  bool isOpen() const { return is_open_; }

private:
  int getMajorGroup(uint32_t version) const;
  bool matchesFilter(const Domain::ClientVersion &version) const;
  void populateTreeData();
  void renderToolbar();
  void renderLeftPane();
  void renderRightPane();
  void renderFooter();
  void selectClient(uint32_t version);
  void runAssetDetection();

  void addClient();
  void duplicateClient();
  void deleteClient(uint32_t version);
  bool saveAll();
  void discardChanges();
  bool validateBeforeSave();

  Services::ClientVersionRegistry *registry_ = nullptr;
  Services::ConfigService *config_ = nullptr;
  bool is_open_ = false;

  uint32_t active_version_ = 0;
  char search_buf_[128] = {};
  std::string search_filter_;

  struct TreeGroup {
    int major_version;
    std::string label;
    std::vector<uint32_t> client_versions;
  };
  std::vector<TreeGroup> tree_groups_;

  std::unordered_set<uint32_t> pending_deleted_;
  std::string validation_error_;
  bool check_signatures_ = true;

  // Extracted property editor component (owns buffers, detection state, color states)
  std::unique_ptr<ClientPropertyEditor> editor_;
};

} // namespace UI
} // namespace MapEditor
