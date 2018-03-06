#include <stdio.h>
#include <string.h>

#include "openxbox/core.h"
#include "openxbox/settings.h"

// Debugging stuff
#define DUMP_XBE_INFO 0

#ifdef _WIN32
char *basename(char *path)
{
	char name[MAX_PATH];
	char ext[MAX_PATH];
	if (_splitpath_s(path, NULL, 0, NULL, 0, name, MAX_PATH, ext, MAX_PATH)) {
		return NULL;
	}
	size_t len = strlen(name) + strlen(ext) + 1;
	char *out = (char *)malloc(len);
	strcpy(out, name);
	strcat(out, ext);
	return out;
}
#endif

/*!
 * Program entry point
 */
int main(int argc, const char *argv[]) {
	using namespace openxbox;

	auto info = GetOpenXBOXInfo();
	printf("OpenXBOX v%s\n", info->version);
	printf("------------------\n");

    const char *xbe_path;
    const char *usage = "usage: %s <xbe>\n";

    if (argc < 2) {
        printf(usage, basename((char*)argv[0]));
        return 1;
    }

	// Locate and instantiate modules
	ModuleRepository moduleRepo;
	moduleRepo.Enumerate();

	// Search for CPU modules
	auto cpuModules = moduleRepo.GetCPUModules();
	if (cpuModules.size() == 0) {
		log_fatal("No CPU modules found\n");
		return -1;
	}

	// Load first CPU module found
	auto cpuModuleInfo = cpuModules[0];

	// Instantiate CPU module
	log_info("Loading CPU module: %s %s... ", cpuModuleInfo.moduleName.c_str(), cpuModuleInfo.moduleVersion.c_str());
	CPUModuleInstance cpuModuleInstance;
	auto cpuModuleResult = LoadCPUModule(cpuModuleInfo.libraryPath, &cpuModuleInstance);
	if (cpuModuleResult != kModuleLoadSuccess) {
		log_fatal("instantiation failed; result code 0x%x\n", cpuModuleResult);
		return -1;
	}
	log_info("success\n");

    // Load XBE executable
    xbe_path = argv[1];
    Xbe *xbe = new Xbe(xbe_path);
    if (xbe->GetError() != 0) {
		delete xbe;
        return 1;
    }

#if DUMP_XBE_INFO
    xbe->DumpInformation(stdout);
#endif

    // TODO: move this to Xbox class
    FILE *fp;
    errno_t e;
    long sz;

    // Load MCPX ROM
    e = fopen_s(&fp, "path/to/mcpx.bin", "rb");
    if (e) {
        delete xbe;
        return 1;
    }
    fseek(fp, 0, SEEK_END);
    sz = ftell(fp);
    fseek(fp, 0, SEEK_SET);
    if (sz != 512) {
        fprintf(stderr, "MCPX ROM size invalid\n");
        fclose(fp);
        return 1;
    }
    char *mcpx = new char[sz];
    fread_s(mcpx, sz, 1, sz, fp);
    fclose(fp);

    // Load BIOS ROM
    e = fopen_s(&fp, "path/to/bios.bin", "rb");
    if (e) {
        delete xbe;
        return 1;
    }
    fseek(fp, 0, SEEK_END);
    sz = ftell(fp);
    if (sz != MiB(1) && sz != KiB(256)) {
        fprintf(stderr, "BIOS ROM size invalid\n");
        fclose(fp);
        return 1;
    }
    fseek(fp, 0, SEEK_SET);
    char *bios = new char[sz];
    fread_s(bios, sz, 1, sz, fp);
    fclose(fp);

    uint32_t biosSize = sz;

    OpenXBOXSettings settings;
    settings.cpu_singleStep = false;
    settings.debug_dumpMemoryMapping = true;
    settings.debug_dumpXBESectionContents = false;
    settings.gdb_enable = false;

	int result = 0;
    Xbox *xbox = new Xbox(cpuModuleInstance.cpuModule);
	if (xbox->Initialize(&settings)) {
		log_fatal("Xbox initialization failed\n");
		result = -2;
		goto exit;
	}
	/*if (xbox->LoadXbe(xbe)) {
		log_fatal("XBE load failed\n");
		result = -3;
		goto exit;
	}*/
	xbox->InitializePreRun();
    xbox->LoadROMs(mcpx, bios, biosSize);
    xbox->Run();
	xbox->Cleanup();

exit:
	delete xbox;
    delete xbe;
    return result;
}