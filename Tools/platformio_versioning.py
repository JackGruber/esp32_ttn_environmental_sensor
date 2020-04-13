import sys
sys.path.insert(0, 'C:\\git\\meine\\auto_version')
import versioning
versioning.UpdateVersionFile("include/version.h", "DEFINEHEADER", False, "include/version_build.h")