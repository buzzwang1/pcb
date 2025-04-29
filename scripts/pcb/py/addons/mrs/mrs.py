# Will be called by toolcaller via the project's configuration
# System class is passed, to get the project to get access to the projects's configuration


import os, shutil, sys, re, json

class MyClass:
    def __init__(self, lcSystem):
        self.mcSystem  = lcSystem
        self.mcProject = self.mcSystem.Project_Get()

        self.lbDebug = True

    def run(self, lszParameter):

        # Get the data from the configuration
        dir_root =        self.mcProject.isKeyOrDie("system/dir/root")
        dir_cfg_project = os.path.join(self.mcProject.isKeyOrDie("system/dir/project/build/bin"), "")

        lszProjectName = self.mcProject.GetName()
        self.debug(lszProjectName)

        lszMRSFile = os.path.join(dir_cfg_project, lszProjectName + ".wvproj")
        lszMRS_build_output        = self.mcProject.isKeyOrDie("system/file/elf")
        lszMRS_build_object_output = self.mcProject.isKeyOrDie("system/dir/project/build/bin")
        lszMRS_Hex_filename        = os.path.join(lszMRS_build_object_output, lszProjectName + ".elf.hex")
        lszMRS_debug_interface_svd = self.mcProject.isKeyOrDie("system/project/target/file_svd")
        lszMRS_build_cmake         = self.mcProject.isKeyOrDie("system/dir/project/build/cmake")
        lszMRS_build_root          = self.mcProject.isKeyOrDie("system/dir/project/build/root")
        lszMRS_build_gen           = self.mcProject.isKeyOrDie("system/dir/project/build/gen")
        lszMRS_PkgLst              = self.mcProject.isKeyOrDie("codegen/CreatedPkgList")

        self.mcSystem.Call(self.mcProject.isKeyOrDie("codegen/cmake_create_pkg_list/call"), "")

        self.mcSystem.mcFile.CreateDirForFile(lszMRSFile)

        self.debug("MRS File:            " + lszMRSFile)
        self.debug("Build Output:        " + lszMRS_build_output)
        self.debug("Build Object Output: " + lszMRS_build_object_output)
        self.debug("Hex Filename:        " + lszMRS_Hex_filename)
        self.debug("Debug Interface svd  " + lszMRS_debug_interface_svd)

        lszMRS_MCU_Type = "CH32V30x"
        if ("641" in lszMRS_debug_interface_svd):
            lszMRS_MCU_Type = "CH641"

        self.debug("MRS MCU Type         " + lszMRS_MCU_Type)

        self.debug("")
        self.debug("Copying project files")
        self.mcSystem.mcFile.Copy(os.path.join(os.path.dirname(os.path.abspath(__file__)), "dummy.wvproj") , lszMRSFile)
        #self.mcSystem.mcFile.Copy(lszMRS_Hex_filename, os.path.join(dir_cfg_project, "obj", lszProjectName + ".hex"))
        #self.mcSystem.mcFile.Copy(lszMRS_build_output, os.path.join(dir_cfg_project, "obj", lszProjectName + ".elf"))


        # ------------ Create the file in a string to be compareable with a existing file
        lszFileName = os.path.join(dir_cfg_project, ".template")
        self.debug("Creating: " + lszFileName)
        lszFile = ""

        if lszMRS_MCU_Type == "CH32V30x":
            lszFile += 'Mcu Type=CH32V30x\n'
            lszFile += 'Address=0x08000000\n'
            lszFile += 'Erase All=true\n'
            lszFile += 'Program=true\n'
            lszFile += 'Verify=true\n'
            lszFile += 'Reset=true\n'
            lszFile += '\n'
            lszFile += 'Vendor=WCH\n'
            lszFile += 'Link=WCH-Link\n'
            lszFile += 'Toolchain=RISC-V\n'
            lszFile += 'Series=CH32V307\n'
            lszFile += 'Description=ROM(byte): 256K, SRAM(byte): 64K, CHIP PINS: 64, GPIO PORTS: 51.\\nWCH CH32V3 series of mainstream MCUs covers the needs of a large variety of applications in the industrial,medical and consumer markets. High performance with first-class peripherals and low-power,low-voltage operation is paired with a high level of integration at accessible prices with a simple architecture and easy-to-use tools.\n'
            lszFile += '\n'
            lszFile += 'PeripheralVersion=1.4\n'
            lszFile += 'Target Path=' + lszProjectName + 'elf.hex\n'
            lszFile += 'MCU=CH32V307RVT6\n'

        if lszMRS_MCU_Type == "CH641":
            lszFile += 'Mcu Type=CH641\n'
            lszFile += 'Address=0x08000000\n'
            lszFile += 'Erase All=true\n'
            lszFile += 'Program=true\n'
            lszFile += 'Verify=true\n'
            lszFile += 'Reset=true\n'
            lszFile += '\n'
            lszFile += 'Vendor=WCH\n'
            lszFile += 'Link=WCH-Link\n'
            lszFile += 'Toolchain=RISC-V\n'
            lszFile += 'Series=CH641\n'
            lszFile += 'Description=ROM(byte): 16K, SRAM(byte): 2K, CHIP PINS: 28, GPIO PORTS: 25.\\nWCH CH641 Series is a special microcontroller for PD wireless charging based on Highland Barley RISC-V2A core design. The series has a wide pressure, single line tone Test, low power consumption, peripheral compact and other features. CH641 Built-in PD PHY, BC interface, differential input current sampling and AC small signal amplification and decoding. It supports USB PD and Type C fast charging function, BC1.2 and DCP and other HV charging protocols, and provides a DMA controller and 10-bit mode Digital conversion ADC, multi-group timer, USART serial port, I2C and other rich peripheral resources, provide overvoltage protection and overtemperature protection.\n'
            lszFile += '\n'
            lszFile += 'PeripheralVersion=1.0\n'
            lszFile += 'Target Path=' + lszProjectName + 'elf.hex\n'
            lszFile += 'MCU=CH641F\n'

        self.mcSystem.mcFile.CreateDirForFile(lszFileName)
        lbWrite =True
        # Check if it is already existing
        if (os.path.exists(os.path.join(lszFileName))):
            with open(os.path.join(lszFileName), "r") as loExistingGenList:
                lbWrite = False
                #lszExistingGenList = loExistingGenList.read()
                #if (lszExistingGenList != lszEmBitzPrj):
                    #lbWrite = True
        else:
            lbWrite = True

        if (lbWrite == True):
            with open(os.path.join(lszFileName), "w") as text_file:
                text_file.write(lszFile)


        # --------------------- Create the file in a string to be compareable with a existing file
        lszFileName = os.path.join(dir_cfg_project, ".project")
        self.debug("Creating: " + lszFileName)
        lszFile = ""
        lszFile += '<?xml version="1.0" encoding="UTF-8" standalone="no"?>\n'
        lszFile += '<projectDescription>\n'
        lszFile += '  <name>' + lszProjectName + '</name>\n'
        lszFile += '  <natures>\n'
        lszFile += '    <nature>org.eclipse.cdt.core.cnature</nature>\n'
        lszFile += '    <nature>org.eclipse.cdt.managedbuilder.core.managedBuildNature</nature>\n'
        lszFile += '    <nature>org.eclipse.cdt.managedbuilder.core.ScannerConfigNature</nature>\n'
        lszFile += '  </natures>\n'
        lszFile += '</projectDescription>\n'

        self.mcSystem.mcFile.CreateDirForFile(lszFileName)
        lbWrite =True
        # Check if it is already existing
        if (os.path.exists(os.path.join(lszFileName))):
            with open(os.path.join(lszFileName), "r") as loExistingGenList:
                lbWrite = False
                #lszExistingGenList = loExistingGenList.read()
                #if (lszExistingGenList != lszEmBitzPrj):
                    #lbWrite = True
        else:
            lbWrite = True

        if (lbWrite == True):
            with open(os.path.join(lszFileName), "w") as text_file:
                text_file.write(lszFile)


        # --------------------- Create the file in a string to be compareable with a existing file
        lszFileName = os.path.join(dir_cfg_project, ".cproject")
        self.debug("Creating: " + lszFileName)
        lszFile = ""
        lszFile += '<?xml version="1.0" encoding="UTF-8" standalone="no"?>\n'
        lszFile += '<?fileVersion 4.0.0?><cproject storage_type_id="org.eclipse.cdt.core.XmlProjectDescriptionStorage">\n'
        lszFile += '  <storageModule moduleId="org.eclipse.cdt.core.settings">\n'
        lszFile += '    <cconfiguration id="ilg.gnumcueclipse.managedbuild.cross.riscv.config.elf.release.1008047074">\n'
        lszFile += '      <storageModule buildSystemId="org.eclipse.cdt.managedbuilder.core.configurationDataProvider" id="ilg.gnumcueclipse.managedbuild.cross.riscv.config.elf.release.1008047074" moduleId="org.eclipse.cdt.core.settings" name="obj">\n'
        lszFile += '        <externalSettings/>\n'
        lszFile += '        <extensions>\n'
        lszFile += '          <extension id="org.eclipse.cdt.core.ELF" point="org.eclipse.cdt.core.BinaryParser"/>\n'
        lszFile += '          <extension id="org.eclipse.cdt.core.GASErrorParser" point="org.eclipse.cdt.core.ErrorParser"/>\n'
        lszFile += '          <extension id="org.eclipse.cdt.core.GmakeErrorParser" point="org.eclipse.cdt.core.ErrorParser"/>\n'
        lszFile += '          <extension id="org.eclipse.cdt.core.GLDErrorParser" point="org.eclipse.cdt.core.ErrorParser"/>\n'
        lszFile += '          <extension id="org.eclipse.cdt.core.CWDLocator" point="org.eclipse.cdt.core.ErrorParser"/>\n'
        lszFile += '          <extension id="org.eclipse.cdt.core.GCCErrorParser" point="org.eclipse.cdt.core.ErrorParser"/>\n'
        lszFile += '        </extensions>\n'
        lszFile += '      </storageModule>\n'
        lszFile += '      <storageModule moduleId="cdtBuildSystem" version="4.0.0">\n'
        lszFile += '        <configuration artifactName="${ProjName}" buildArtefactType="org.eclipse.cdt.build.core.buildArtefactType.exe" buildProperties="org.eclipse.cdt.build.core.buildArtefactType=org.eclipse.cdt.build.core.buildArtefactType.exe,org.eclipse.cdt.build.core.buildType=org.eclipse.cdt.build.core.buildType.release" cleanCommand="${cross_rm} -rf" description="" id="ilg.gnumcueclipse.managedbuild.cross.riscv.config.elf.release.1008047074" name="obj" parent="ilg.gnumcueclipse.managedbuild.cross.riscv.config.elf.release">\n'
        lszFile += '          <folderInfo id="ilg.gnumcueclipse.managedbuild.cross.riscv.config.elf.release.1008047074." name="/" resourcePath="">\n'
        lszFile += '            <toolChain id="ilg.gnumcueclipse.managedbuild.cross.riscv.toolchain.elf.release.231146001" name="RISC-V Cross GCC" superClass="ilg.gnumcueclipse.managedbuild.cross.riscv.toolchain.elf.release">\n'
        lszFile += '              <option id="ilg.gnumcueclipse.managedbuild.cross.riscv.option.addtools.createflash.1311852988" name="Create flash image" superClass="ilg.gnumcueclipse.managedbuild.cross.riscv.option.addtools.createflash" useByScannerDiscovery="false" value="true" valueType="boolean"/>\n'
        lszFile += '              <option id="ilg.gnumcueclipse.managedbuild.cross.riscv.option.addtools.createlisting.1983282875" name="Create extended listing" superClass="ilg.gnumcueclipse.managedbuild.cross.riscv.option.addtools.createlisting" useByScannerDiscovery="false" value="true" valueType="boolean"/>\n'
        lszFile += '              <option id="ilg.gnumcueclipse.managedbuild.cross.riscv.option.addtools.printsize.1000761142" name="Print size" superClass="ilg.gnumcueclipse.managedbuild.cross.riscv.option.addtools.printsize" useByScannerDiscovery="false" value="true" valueType="boolean"/>\n'
        lszFile += '              <option id="ilg.gnumcueclipse.managedbuild.cross.riscv.option.optimization.level.514997414" name="Optimization Level" superClass="ilg.gnumcueclipse.managedbuild.cross.riscv.option.optimization.level" useByScannerDiscovery="true" value="ilg.gnumcueclipse.managedbuild.cross.riscv.option.optimization.level.size" valueType="enumerated"/>\n'
        lszFile += '              <option id="ilg.gnumcueclipse.managedbuild.cross.riscv.option.optimization.messagelength.1008570639" name="Message length (-fmessage-length=0)" superClass="ilg.gnumcueclipse.managedbuild.cross.riscv.option.optimization.messagelength" useByScannerDiscovery="true" value="true" valueType="boolean"/>\n'
        lszFile += '              <option id="ilg.gnumcueclipse.managedbuild.cross.riscv.option.optimization.signedchar.467272439" name="\'char\' is signed (-fsigned-char)" superClass="ilg.gnumcueclipse.managedbuild.cross.riscv.option.optimization.signedchar" useByScannerDiscovery="true" value="true" valueType="boolean"/>\n'
        lszFile += '              <option id="ilg.gnumcueclipse.managedbuild.cross.riscv.option.optimization.functionsections.2047756949" name="Function sections (-ffunction-sections)" superClass="ilg.gnumcueclipse.managedbuild.cross.riscv.option.optimization.functionsections" useByScannerDiscovery="true" value="true" valueType="boolean"/>\n'
        lszFile += '              <option id="ilg.gnumcueclipse.managedbuild.cross.riscv.option.optimization.datasections.207613650" name="Data sections (-fdata-sections)" superClass="ilg.gnumcueclipse.managedbuild.cross.riscv.option.optimization.datasections" useByScannerDiscovery="true" value="true" valueType="boolean"/>\n'
        lszFile += '              <option id="ilg.gnumcueclipse.managedbuild.cross.riscv.option.debugging.level.1204865254" name="Debug level" superClass="ilg.gnumcueclipse.managedbuild.cross.riscv.option.debugging.level" useByScannerDiscovery="true"/>\n'
        lszFile += '              <option id="ilg.gnumcueclipse.managedbuild.cross.riscv.option.debugging.format.867779652" name="Debug format" superClass="ilg.gnumcueclipse.managedbuild.cross.riscv.option.debugging.format" useByScannerDiscovery="true"/>\n'
        lszFile += '              <option id="ilg.gnumcueclipse.managedbuild.cross.riscv.option.target.isa.base.1900297968" name="Architecture" superClass="ilg.gnumcueclipse.managedbuild.cross.riscv.option.target.isa.base" useByScannerDiscovery="false" value="ilg.gnumcueclipse.managedbuild.cross.riscv.option.target.arch.rv32i" valueType="enumerated"/>\n'
        lszFile += '              <option id="ilg.gnumcueclipse.managedbuild.cross.riscv.option.target.abi.integer.387605487" name="Integer ABI" superClass="ilg.gnumcueclipse.managedbuild.cross.riscv.option.target.abi.integer" useByScannerDiscovery="false" value="ilg.gnumcueclipse.managedbuild.cross.riscv.option.abi.integer.ilp32" valueType="enumerated"/>\n'
        lszFile += '              <option id="ilg.gnumcueclipse.managedbuild.cross.riscv.option.target.isa.multiply.1509705449" name="Multiply extension (RVM)" superClass="ilg.gnumcueclipse.managedbuild.cross.riscv.option.target.isa.multiply" useByScannerDiscovery="false" value="true" valueType="boolean"/>\n'
        lszFile += '              <option id="ilg.gnumcueclipse.managedbuild.cross.riscv.option.target.isa.compressed.1038505275" name="Compressed extension (RVC)" superClass="ilg.gnumcueclipse.managedbuild.cross.riscv.option.target.isa.compressed" useByScannerDiscovery="false" value="true" valueType="boolean"/>\n'
        lszFile += '              <option id="ilg.gnumcueclipse.managedbuild.cross.riscv.option.toolchain.name.1218760634" superClass="ilg.gnumcueclipse.managedbuild.cross.riscv.option.toolchain.name" useByScannerDiscovery="false" value="GNU MCU RISC-V GCC" valueType="string"/>\n'
        lszFile += '              <option id="ilg.gnumcueclipse.managedbuild.cross.riscv.option.command.prefix.103341323" name="Prefix" superClass="ilg.gnumcueclipse.managedbuild.cross.riscv.option.command.prefix" useByScannerDiscovery="false" value="riscv-none-embed-" valueType="string"/>\n'
        lszFile += '              <option id="ilg.gnumcueclipse.managedbuild.cross.riscv.option.command.c.487601824" name="C compiler" superClass="ilg.gnumcueclipse.managedbuild.cross.riscv.option.command.c" useByScannerDiscovery="false" value="gcc" valueType="string"/>\n'
        lszFile += '              <option id="ilg.gnumcueclipse.managedbuild.cross.riscv.option.command.cpp.1062130429" name="C++ compiler" superClass="ilg.gnumcueclipse.managedbuild.cross.riscv.option.command.cpp" useByScannerDiscovery="false" value="g++" valueType="string"/>\n'
        lszFile += '              <option id="ilg.gnumcueclipse.managedbuild.cross.riscv.option.command.ar.1194282993" name="Archiver" superClass="ilg.gnumcueclipse.managedbuild.cross.riscv.option.command.ar" useByScannerDiscovery="false" value="ar" valueType="string"/>\n'
        lszFile += '              <option id="ilg.gnumcueclipse.managedbuild.cross.riscv.option.command.objcopy.1529355265" name="Hex/Bin converter" superClass="ilg.gnumcueclipse.managedbuild.cross.riscv.option.command.objcopy" useByScannerDiscovery="false" value="objcopy" valueType="string"/>\n'
        lszFile += '              <option id="ilg.gnumcueclipse.managedbuild.cross.riscv.option.command.objdump.1053750745" name="Listing generator" superClass="ilg.gnumcueclipse.managedbuild.cross.riscv.option.command.objdump" useByScannerDiscovery="false" value="objdump" valueType="string"/>\n'
        lszFile += '              <option id="ilg.gnumcueclipse.managedbuild.cross.riscv.option.command.size.1441326233" name="Size command" superClass="ilg.gnumcueclipse.managedbuild.cross.riscv.option.command.size" useByScannerDiscovery="false" value="size" valueType="string"/>\n'
        lszFile += '              <option id="ilg.gnumcueclipse.managedbuild.cross.riscv.option.command.make.550105535" name="Build command" superClass="ilg.gnumcueclipse.managedbuild.cross.riscv.option.command.make" useByScannerDiscovery="false" value="make" valueType="string"/>\n'
        lszFile += '              <option id="ilg.gnumcueclipse.managedbuild.cross.riscv.option.command.rm.719280496" name="Remove command" superClass="ilg.gnumcueclipse.managedbuild.cross.riscv.option.command.rm" useByScannerDiscovery="false" value="rm" valueType="string"/>\n'
        lszFile += '              <option id="ilg.gnumcueclipse.managedbuild.cross.riscv.option.toolchain.id.226017994" superClass="ilg.gnumcueclipse.managedbuild.cross.riscv.option.toolchain.id" useByScannerDiscovery="false" value="512258282" valueType="string"/>\n'
        lszFile += '              <option id="ilg.gnumcueclipse.managedbuild.cross.riscv.option.target.isa.atomic.1590833110" name="Atomic extension (RVA)" superClass="ilg.gnumcueclipse.managedbuild.cross.riscv.option.target.isa.atomic" useByScannerDiscovery="false" value="true" valueType="boolean"/>\n'
        lszFile += '              <option id="ilg.gnumcueclipse.managedbuild.cross.riscv.option.warnings.unused.1961191588" name="Warn on various unused elements (-Wunused)" superClass="ilg.gnumcueclipse.managedbuild.cross.riscv.option.warnings.unused" useByScannerDiscovery="true" value="true" valueType="boolean"/>\n'
        lszFile += '              <option id="ilg.gnumcueclipse.managedbuild.cross.riscv.option.warnings.uninitialized.929829166" name="Warn on uninitialized variables (-Wuninitialised)" superClass="ilg.gnumcueclipse.managedbuild.cross.riscv.option.warnings.uninitialized" useByScannerDiscovery="true" value="true" valueType="boolean"/>\n'
        lszFile += '              <option id="ilg.gnumcueclipse.managedbuild.cross.riscv.option.target.isa.xw.973711437" name="Extra Compressed extension (RVXW)" superClass="ilg.gnumcueclipse.managedbuild.cross.riscv.option.target.isa.xw" value="true" valueType="boolean"/>\n'
        lszFile += '              <option id="ilg.gnumcueclipse.managedbuild.cross.riscv.option.target.saverestore.433339372" name="Small prologue/epilogue (-msave-restore)" superClass="ilg.gnumcueclipse.managedbuild.cross.riscv.option.target.saverestore" value="true" valueType="boolean"/>\n'
        lszFile += '              <option id="ilg.gnumcueclipse.managedbuild.cross.riscv.option.optimization.nocommon.2081710805" name="No common unitialized (-fno-common)" superClass="ilg.gnumcueclipse.managedbuild.cross.riscv.option.optimization.nocommon" value="true" valueType="boolean"/>\n'
        lszFile += '              <targetPlatform archList="all" binaryParser="org.eclipse.cdt.core.ELF" id="ilg.gnumcueclipse.managedbuild.cross.riscv.targetPlatform.1944008784" isAbstract="false" osList="all" superClass="ilg.gnumcueclipse.managedbuild.cross.riscv.targetPlatform"/>\n'
        lszFile += '              <builder buildPath="${workspace_loc:/' + lszProjectName + '}/obj" id="ilg.gnumcueclipse.managedbuild.cross.riscv.builder.1421508906" keepEnvironmentInBuildfile="false" managedBuildOn="true" name="GNU Make" parallelBuildOn="true" parallelizationNumber="optimal" superClass="ilg.gnumcueclipse.managedbuild.cross.riscv.builder"/>\n'
        lszFile += '              <tool id="ilg.gnumcueclipse.managedbuild.cross.riscv.tool.assembler.1244756189" name="GNU RISC-V Cross Assembler" superClass="ilg.gnumcueclipse.managedbuild.cross.riscv.tool.assembler">\n'
        lszFile += '                <option id="ilg.gnumcueclipse.managedbuild.cross.riscv.option.assembler.usepreprocessor.1692176068" name="Use preprocessor" superClass="ilg.gnumcueclipse.managedbuild.cross.riscv.option.assembler.usepreprocessor" useByScannerDiscovery="false" value="true" valueType="boolean"/>\n'
        lszFile += '                <option IS_BUILTIN_EMPTY="false" IS_VALUE_EMPTY="false" id="ilg.gnumcueclipse.managedbuild.cross.riscv.option.assembler.include.paths.1034038285" name="Include paths (-I)" superClass="ilg.gnumcueclipse.managedbuild.cross.riscv.option.assembler.include.paths" useByScannerDiscovery="true" valueType="includePath">\n'
        lszFile += '                  <listOptionValue builtIn="false" value="&quot;${workspace_loc:/${ProjName}/Startup}&quot;"/>\n'
        lszFile += '                </option>\n'
        lszFile += '                <inputType id="ilg.gnumcueclipse.managedbuild.cross.riscv.tool.assembler.input.126366858" superClass="ilg.gnumcueclipse.managedbuild.cross.riscv.tool.assembler.input"/>\n'
        lszFile += '              </tool>\n'
        lszFile += '              <tool id="ilg.gnumcueclipse.managedbuild.cross.riscv.tool.c.compiler.1731377187" name="GNU RISC-V Cross C Compiler" superClass="ilg.gnumcueclipse.managedbuild.cross.riscv.tool.c.compiler">\n'
        lszFile += '                <option IS_BUILTIN_EMPTY="false" IS_VALUE_EMPTY="false" id="ilg.gnumcueclipse.managedbuild.cross.riscv.option.c.compiler.include.paths.1567947810" name="Include paths (-I)" superClass="ilg.gnumcueclipse.managedbuild.cross.riscv.option.c.compiler.include.paths" useByScannerDiscovery="true" valueType="includePath">\n'
        lszFile += '                  <listOptionValue builtIn="false" value="&quot;${workspace_loc:/${ProjName}/Debug}&quot;"/>\n'
        lszFile += '                  <listOptionValue builtIn="false" value="&quot;${workspace_loc:/${ProjName}/Core}&quot;"/>\n'
        lszFile += '                  <listOptionValue builtIn="false" value="&quot;${workspace_loc:/${ProjName}/User}&quot;"/>\n'
        lszFile += '                  <listOptionValue builtIn="false" value="&quot;${workspace_loc:/${ProjName}/Peripheral/inc}&quot;"/>\n'
        lszFile += '                </option>\n'
        lszFile += '                <option id="ilg.gnumcueclipse.managedbuild.cross.riscv.option.c.compiler.std.2020844713" name="Language standard" superClass="ilg.gnumcueclipse.managedbuild.cross.riscv.option.c.compiler.std" useByScannerDiscovery="true" value="ilg.gnumcueclipse.managedbuild.cross.riscv.option.c.compiler.std.gnu99" valueType="enumerated"/>\n'
        lszFile += '                <option IS_BUILTIN_EMPTY="false" IS_VALUE_EMPTY="true" id="ilg.gnumcueclipse.managedbuild.cross.riscv.option.c.compiler.defs.177116515" name="Defined symbols (-D)" superClass="ilg.gnumcueclipse.managedbuild.cross.riscv.option.c.compiler.defs" useByScannerDiscovery="true" valueType="definedSymbols"/>\n'
        lszFile += '                <inputType id="ilg.gnumcueclipse.managedbuild.cross.riscv.tool.c.compiler.input.2036806839" superClass="ilg.gnumcueclipse.managedbuild.cross.riscv.tool.c.compiler.input"/>\n'
        lszFile += '              </tool>\n'
        lszFile += '              <tool id="ilg.gnumcueclipse.managedbuild.cross.riscv.tool.cpp.compiler.1610882921" name="GNU RISC-V Cross C++ Compiler" superClass="ilg.gnumcueclipse.managedbuild.cross.riscv.tool.cpp.compiler"/>\n'
        lszFile += '              <tool id="ilg.gnumcueclipse.managedbuild.cross.riscv.tool.c.linker.1620074387" name="GNU RISC-V Cross C Linker" superClass="ilg.gnumcueclipse.managedbuild.cross.riscv.tool.c.linker">\n'
        lszFile += '                <option id="ilg.gnumcueclipse.managedbuild.cross.riscv.option.c.linker.gcsections.194760422" name="Remove unused sections (-Xlinker --gc-sections)" superClass="ilg.gnumcueclipse.managedbuild.cross.riscv.option.c.linker.gcsections" useByScannerDiscovery="false" value="true" valueType="boolean"/>\n'
        lszFile += '                <option IS_BUILTIN_EMPTY="false" IS_VALUE_EMPTY="true" id="ilg.gnumcueclipse.managedbuild.cross.riscv.option.c.linker.paths.2057340378" name="Library search path (-L)" superClass="ilg.gnumcueclipse.managedbuild.cross.riscv.option.c.linker.paths" useByScannerDiscovery="false" valueType="libPaths"/>\n'
        lszFile += '                <option IS_BUILTIN_EMPTY="false" IS_VALUE_EMPTY="false" id="ilg.gnumcueclipse.managedbuild.cross.riscv.option.c.linker.scriptfile.1390103472" name="Script files (-T)" superClass="ilg.gnumcueclipse.managedbuild.cross.riscv.option.c.linker.scriptfile" useByScannerDiscovery="false" valueType="stringList">\n'
        lszFile += '                  <listOptionValue builtIn="false" value="&quot;${workspace_loc:/${ProjName}/Ld/Link.ld}&quot;"/>\n'
        lszFile += '                </option>\n'
        lszFile += '                <option id="ilg.gnumcueclipse.managedbuild.cross.riscv.option.c.linker.nostart.913830613" name="Do not use standard start files (-nostartfiles)" superClass="ilg.gnumcueclipse.managedbuild.cross.riscv.option.c.linker.nostart" useByScannerDiscovery="false" value="true" valueType="boolean"/>\n'
        lszFile += '                <option id="ilg.gnumcueclipse.managedbuild.cross.riscv.option.c.linker.usenewlibnano.239404511" name="Use newlib-nano (--specs=nano.specs)" superClass="ilg.gnumcueclipse.managedbuild.cross.riscv.option.c.linker.usenewlibnano" useByScannerDiscovery="false" value="true" valueType="boolean"/>\n'
        lszFile += '                <option id="ilg.gnumcueclipse.managedbuild.cross.riscv.option.c.linker.usenewlibnosys.351964161" name="Do not use syscalls (--specs=nosys.specs)" superClass="ilg.gnumcueclipse.managedbuild.cross.riscv.option.c.linker.usenewlibnosys" useByScannerDiscovery="false" value="true" valueType="boolean"/>\n'
        lszFile += '                <option IS_BUILTIN_EMPTY="false" IS_VALUE_EMPTY="true" id="ilg.gnumcueclipse.managedbuild.cross.riscv.option.c.linker.otherobjs.16994550" name="Other objects" superClass="ilg.gnumcueclipse.managedbuild.cross.riscv.option.c.linker.otherobjs" useByScannerDiscovery="false" valueType="userObjs"/>\n'
        lszFile += '                <option IS_BUILTIN_EMPTY="false" IS_VALUE_EMPTY="true" id="ilg.gnumcueclipse.managedbuild.cross.riscv.option.c.linker.flags.1125808200" name="Linker flags (-Xlinker [option])" superClass="ilg.gnumcueclipse.managedbuild.cross.riscv.option.c.linker.flags" useByScannerDiscovery="false" valueType="stringList"/>\n'
        lszFile += '                <option IS_BUILTIN_EMPTY="false" IS_VALUE_EMPTY="false" id="ilg.gnumcueclipse.managedbuild.cross.riscv.option.c.linker.libs.1976506231" name="Libraries (-l)" superClass="ilg.gnumcueclipse.managedbuild.cross.riscv.option.c.linker.libs" useByScannerDiscovery="false" valueType="libs">\n'
        lszFile += '                  <listOptionValue builtIn="false" value="m"/>\n'
        lszFile += '                </option>\n'
        lszFile += '                <inputType id="ilg.gnumcueclipse.managedbuild.cross.riscv.tool.c.linker.input.1859223768" superClass="ilg.gnumcueclipse.managedbuild.cross.riscv.tool.c.linker.input">\n'
        lszFile += '                  <additionalInput kind="additionalinputdependency" paths="$(USER_OBJS)"/>\n'
        lszFile += '                  <additionalInput kind="additionalinput" paths="$(LIBS)"/>\n'
        lszFile += '                </inputType>\n'
        lszFile += '              </tool>\n'
        lszFile += '              <tool id="ilg.gnumcueclipse.managedbuild.cross.riscv.tool.cpp.linker.1947503520" name="GNU RISC-V Cross C++ Linker" superClass="ilg.gnumcueclipse.managedbuild.cross.riscv.tool.cpp.linker">\n'
        lszFile += '                <option id="ilg.gnumcueclipse.managedbuild.cross.riscv.option.cpp.linker.gcsections.1689063433" name="Remove unused sections (-Xlinker --gc-sections)" superClass="ilg.gnumcueclipse.managedbuild.cross.riscv.option.cpp.linker.gcsections" value="true" valueType="boolean"/>\n'
        lszFile += '                <option IS_BUILTIN_EMPTY="false" IS_VALUE_EMPTY="false" id="ilg.gnumcueclipse.managedbuild.cross.riscv.option.cpp.linker.paths.1029177148" name="Library search path (-L)" superClass="ilg.gnumcueclipse.managedbuild.cross.riscv.option.cpp.linker.paths" valueType="libPaths">\n'
        lszFile += '                  <listOptionValue builtIn="false" value="&quot;../LD&quot;"/>\n'
        lszFile += '                </option>\n'
        lszFile += '                <option IS_BUILTIN_EMPTY="false" IS_VALUE_EMPTY="false" id="ilg.gnumcueclipse.managedbuild.cross.riscv.option.cpp.linker.scriptfile.1751226764" name="Script files (-T)" superClass="ilg.gnumcueclipse.managedbuild.cross.riscv.option.cpp.linker.scriptfile" valueType="stringList">\n'
        lszFile += '                  <listOptionValue builtIn="false" value="Link.ld"/>\n'
        lszFile += '                </option>\n'
        lszFile += '                <option id="ilg.gnumcueclipse.managedbuild.cross.riscv.option.cpp.linker.nostart.642896175" name="Do not use standard start files (-nostartfiles)" superClass="ilg.gnumcueclipse.managedbuild.cross.riscv.option.cpp.linker.nostart" value="true" valueType="boolean"/>\n'
        lszFile += '                <option id="ilg.gnumcueclipse.managedbuild.cross.riscv.option.cpp.linker.usenewlibnano.1540675679" name="Use newlib-nano (--specs=nano.specs)" superClass="ilg.gnumcueclipse.managedbuild.cross.riscv.option.cpp.linker.usenewlibnano" value="true" valueType="boolean"/>\n'
        lszFile += '              </tool>\n'
        lszFile += '              <tool id="ilg.gnumcueclipse.managedbuild.cross.riscv.tool.archiver.1292785366" name="GNU RISC-V Cross Archiver" superClass="ilg.gnumcueclipse.managedbuild.cross.riscv.tool.archiver"/>\n'
        lszFile += '              <tool id="ilg.gnumcueclipse.managedbuild.cross.riscv.tool.createflash.1801165667" name="GNU RISC-V Cross Create Flash Image" superClass="ilg.gnumcueclipse.managedbuild.cross.riscv.tool.createflash"/>\n'
        lszFile += '              <tool id="ilg.gnumcueclipse.managedbuild.cross.riscv.tool.createlisting.1356766765" name="GNU RISC-V Cross Create Listing" superClass="ilg.gnumcueclipse.managedbuild.cross.riscv.tool.createlisting">\n'
        lszFile += '                <option id="ilg.gnumcueclipse.managedbuild.cross.riscv.option.createlisting.source.2052761852" name="Display source (--source|-S)" superClass="ilg.gnumcueclipse.managedbuild.cross.riscv.option.createlisting.source" useByScannerDiscovery="false" value="false" valueType="boolean"/>\n'
        lszFile += '                <option id="ilg.gnumcueclipse.managedbuild.cross.riscv.option.createlisting.allheaders.439659821" name="Display all headers (--all-headers|-x)" superClass="ilg.gnumcueclipse.managedbuild.cross.riscv.option.createlisting.allheaders" useByScannerDiscovery="false" value="true" valueType="boolean"/>\n'
        lszFile += '                <option id="ilg.gnumcueclipse.managedbuild.cross.riscv.option.createlisting.demangle.67111865" name="Demangle names (--demangle|-C)" superClass="ilg.gnumcueclipse.managedbuild.cross.riscv.option.createlisting.demangle" useByScannerDiscovery="false" value="true" valueType="boolean"/>\n'
        lszFile += '                <option id="ilg.gnumcueclipse.managedbuild.cross.riscv.option.createlisting.linenumbers.1549373929" name="Display line numbers (--line-numbers|-l)" superClass="ilg.gnumcueclipse.managedbuild.cross.riscv.option.createlisting.linenumbers" useByScannerDiscovery="false" value="false" valueType="boolean"/>\n'
        lszFile += '                <option id="ilg.gnumcueclipse.managedbuild.cross.riscv.option.createlisting.wide.1298918921" name="Wide lines (--wide|-w)" superClass="ilg.gnumcueclipse.managedbuild.cross.riscv.option.createlisting.wide" useByScannerDiscovery="false" value="false" valueType="boolean"/>\n'
        lszFile += '                <option id="ilg.gnumcueclipse.managedbuild.cross.riscv.option.createlisting.disassemble.1859590835" name="Disassemble (--disassemble|-d)" superClass="ilg.gnumcueclipse.managedbuild.cross.riscv.option.createlisting.disassemble" useByScannerDiscovery="false" value="true" valueType="boolean"/>\n'
        lszFile += '              </tool>\n'
        lszFile += '              <tool id="ilg.gnumcueclipse.managedbuild.cross.riscv.tool.printsize.712424314" name="GNU RISC-V Cross Print Size" superClass="ilg.gnumcueclipse.managedbuild.cross.riscv.tool.printsize">\n'
        lszFile += '                <option id="ilg.gnumcueclipse.managedbuild.cross.riscv.option.printsize.format.1404031980" name="Size format" superClass="ilg.gnumcueclipse.managedbuild.cross.riscv.option.printsize.format" useByScannerDiscovery="false"/>\n'
        lszFile += '              </tool>\n'
        lszFile += '            </toolChain>\n'
        lszFile += '          </folderInfo>\n'
        lszFile += '          <sourceEntries>\n'
        lszFile += '            <entry excluding="Startup|Peripheral|Ld|Debug|Core" flags="VALUE_WORKSPACE_PATH|RESOLVED" kind="sourcePath" name=""/>\n'
        lszFile += '            <entry flags="VALUE_WORKSPACE_PATH|RESOLVED" kind="sourcePath" name="Core"/>\n'
        lszFile += '            <entry flags="VALUE_WORKSPACE_PATH|RESOLVED" kind="sourcePath" name="Debug"/>\n'
        lszFile += '            <entry flags="VALUE_WORKSPACE_PATH|RESOLVED" kind="sourcePath" name="Ld"/>\n'
        lszFile += '            <entry flags="VALUE_WORKSPACE_PATH|RESOLVED" kind="sourcePath" name="Peripheral"/>\n'
        lszFile += '            <entry excluding="startup_ch32v30x_D8.S" flags="VALUE_WORKSPACE_PATH|RESOLVED" kind="sourcePath" name="Startup"/>\n'
        lszFile += '          </sourceEntries>\n'
        lszFile += '        </configuration>\n'
        lszFile += '      </storageModule>\n'
        lszFile += '      <storageModule moduleId="org.eclipse.cdt.core.externalSettings"/>\n'
        lszFile += '      <storageModule moduleId="ilg.gnumcueclipse.managedbuild.packs"/>\n'
        lszFile += '    </cconfiguration>\n'
        lszFile += '  </storageModule>\n'
        lszFile += '  <storageModule moduleId="cdtBuildSystem" version="4.0.0">\n'
        lszFile += '    <project id="999.ilg.gnumcueclipse.managedbuild.cross.riscv.target.elf.275846018" name="Executable file" projectType="ilg.gnumcueclipse.managedbuild.cross.riscv.target.elf"/>\n'
        lszFile += '  </storageModule>\n'
        lszFile += '  <storageModule moduleId="scannerConfiguration">\n'
        lszFile += '    <autodiscovery enabled="true" problemReportingEnabled="true" selectedProfileId=""/>\n'
        lszFile += '    <scannerConfigBuildInfo instanceId="ilg.gnumcueclipse.managedbuild.cross.riscv.config.elf.debug.767917625;ilg.gnumcueclipse.managedbuild.cross.riscv.config.elf.debug.767917625.;ilg.gnumcueclipse.managedbuild.cross.riscv.tool.c.compiler.1375371130;ilg.gnumcueclipse.managedbuild.cross.riscv.tool.c.compiler.input.1473381709">\n'
        lszFile += '      <autodiscovery enabled="true" problemReportingEnabled="true" selectedProfileId=""/>\n'
        lszFile += '    </scannerConfigBuildInfo>\n'
        lszFile += '    <scannerConfigBuildInfo instanceId="ilg.gnumcueclipse.managedbuild.cross.riscv.config.elf.release.1008047074;ilg.gnumcueclipse.managedbuild.cross.riscv.config.elf.release.1008047074.;ilg.gnumcueclipse.managedbuild.cross.riscv.tool.c.compiler.1731377187;ilg.gnumcueclipse.managedbuild.cross.riscv.tool.c.compiler.input.2036806839">\n'
        lszFile += '      <autodiscovery enabled="true" problemReportingEnabled="true" selectedProfileId=""/>\n'
        lszFile += '    </scannerConfigBuildInfo>\n'
        lszFile += '  </storageModule>\n'
        lszFile += '  <storageModule moduleId="org.eclipse.cdt.core.LanguageSettingsProviders"/>\n'
        lszFile += '  <storageModule moduleId="org.eclipse.cdt.make.core.buildtargets"/>\n'
        lszFile += '\n'
        lszFile += '</cproject>\n'

        self.mcSystem.mcFile.CreateDirForFile(lszFileName)
        lbWrite =True
        # Check if it is already existing
        if (os.path.exists(os.path.join(lszFileName))):
            with open(os.path.join(lszFileName), "r") as loExistingGenList:
                lbWrite = False
                #lszExistingGenList = loExistingGenList.read()
                #if (lszExistingGenList != lszEmBitzPrj):
                    #lbWrite = True
        else:
            lbWrite = True

        if (lbWrite == True):
            with open(os.path.join(lszFileName), "w") as text_file:
                text_file.write(lszFile)


        # --------------------- Create the list of used packages
        self.mcSystem.CreatePackageGenLists(self.mcProject)


        # --------------------- Create the file in a string to be compareable with a existing file
        lszMRS_Dir_Main = self.mcProject.isKeyOrDie("system/dir/source")
        lszMRS_Dir_Main_Rel = ""
        with open(lszMRS_PkgLst) as lFilePkgLst:
            for lszLine in lFilePkgLst:
                self.debug(lszLine.rstrip())
                if (lszLine.find("main/") > -1):
                    lszMRS_Dir_Main_Rel = lszLine
                    self.debug("***" + lszLine.rstrip())
        lszMRS_Dir_Main = os.path.join(lszMRS_Dir_Main, lszMRS_Dir_Main_Rel)


        lszFileName = os.path.join(dir_cfg_project, lszProjectName+".launch")
        self.debug("Creating: " + lszFileName)
        lszFile = ""
        lszFile += '<?xml version="1.0" encoding="UTF-8" standalone="no"?>\n'
        lszFile += '<launchConfiguration type="com.mounriver.debug.gdbjtag.openocd.launchConfigurationType">\n'
        lszFile += '<booleanAttribute key="com.mounriver.debug.gdbjtag.openocd.doContinue" value="true"/>\n'
        lszFile += '<booleanAttribute key="com.mounriver.debug.gdbjtag.openocd.doDebugInRam" value="false"/>\n'
        lszFile += '<booleanAttribute key="com.mounriver.debug.gdbjtag.openocd.doFirstReset" value="false"/>\n'
        lszFile += '<booleanAttribute key="com.mounriver.debug.gdbjtag.openocd.doGdbServerAllocateConsole" value="true"/>\n'
        lszFile += '<booleanAttribute key="com.mounriver.debug.gdbjtag.openocd.doGdbServerAllocateTelnetConsole" value="false"/>\n'
        lszFile += '<booleanAttribute key="com.mounriver.debug.gdbjtag.openocd.doSecondReset" value="false"/>\n'
        lszFile += '<booleanAttribute key="com.mounriver.debug.gdbjtag.openocd.doStartGdbCLient" value="true"/>\n'
        lszFile += '<booleanAttribute key="com.mounriver.debug.gdbjtag.openocd.doStartGdbServer" value="true"/>\n'
        lszFile += '<booleanAttribute key="com.mounriver.debug.gdbjtag.openocd.enableSemihosting" value="false"/>\n'
        lszFile += '<stringAttribute key="com.mounriver.debug.gdbjtag.openocd.firstResetType" value="init"/>\n'
        lszFile += '<stringAttribute key="com.mounriver.debug.gdbjtag.openocd.gdbClientOtherCommands" value="set mem inaccessible-by-default off&#13;&#10;set architecture riscv:rv32&#13;&#10;set remotetimeout unlimited&#13;&#13;&#10;set disassembler-options xw"/>\n'
        lszFile += '<stringAttribute key="com.mounriver.debug.gdbjtag.openocd.gdbClientOtherOptions" value=""/>\n'
        lszFile += '<stringAttribute key="com.mounriver.debug.gdbjtag.openocd.gdbServerConnectionAddress" value=""/>\n'
        lszFile += '<stringAttribute key="com.mounriver.debug.gdbjtag.openocd.gdbServerExecutable" value="${eclipse_home}toolchain/OpenOCD/bin/openocd.exe"/>\n'
        lszFile += '<intAttribute key="com.mounriver.debug.gdbjtag.openocd.gdbServerGdbPortNumber" value="3333"/>\n'
        lszFile += '<stringAttribute key="com.mounriver.debug.gdbjtag.openocd.gdbServerLog" value=""/>\n'
        lszFile += '<stringAttribute key="com.mounriver.debug.gdbjtag.openocd.gdbServerOther" value="-f &quot;${eclipse_home}toolchain/OpenOCD/bin/wch-riscv.cfg&quot; -c noload"/>\n'
        lszFile += '<stringAttribute key="com.mounriver.debug.gdbjtag.openocd.gdbServerTclPortNumber" value="6666"/>\n'
        lszFile += '<intAttribute key="com.mounriver.debug.gdbjtag.openocd.gdbServerTelnetPortNumber" value="4444"/>\n'
        lszFile += '<stringAttribute key="com.mounriver.debug.gdbjtag.openocd.otherInitCommands" value=""/>\n'
        lszFile += '<stringAttribute key="com.mounriver.debug.gdbjtag.openocd.otherRunCommands" value=""/>\n'
        lszFile += '<stringAttribute key="com.mounriver.debug.gdbjtag.openocd.secondResetType" value="halt"/>\n'
        lszFile += '<stringAttribute key="com.mounriver.debug.gdbjtag.svdPath" value="' + lszMRS_debug_interface_svd + '"/>\n'
        lszFile += '<stringAttribute key="org.eclipse.cdt.debug.gdbjtag.core.imageFileName" value=""/>\n'
        lszFile += '<stringAttribute key="org.eclipse.cdt.debug.gdbjtag.core.imageOffset" value=""/>\n'
        lszFile += '<stringAttribute key="org.eclipse.cdt.debug.gdbjtag.core.ipAddress" value="localhost"/>\n'
        lszFile += '<stringAttribute key="org.eclipse.cdt.debug.gdbjtag.core.jtagDevice" value="GNU MCU OpenOCD"/>\n'
        lszFile += '<booleanAttribute key="org.eclipse.cdt.debug.gdbjtag.core.loadImage" value="true"/>\n'
        lszFile += '<booleanAttribute key="org.eclipse.cdt.debug.gdbjtag.core.loadSymbols" value="true"/>\n'
        lszFile += '<stringAttribute key="org.eclipse.cdt.debug.gdbjtag.core.pcRegister" value=""/>\n'
        lszFile += '<intAttribute key="org.eclipse.cdt.debug.gdbjtag.core.portNumber" value="3333"/>\n'
        lszFile += '<booleanAttribute key="org.eclipse.cdt.debug.gdbjtag.core.setPcRegister" value="false"/>\n'
        lszFile += '<booleanAttribute key="org.eclipse.cdt.debug.gdbjtag.core.setResume" value="false"/>\n'
        lszFile += '<booleanAttribute key="org.eclipse.cdt.debug.gdbjtag.core.setStopAt" value="true"/>\n'
        lszFile += '<stringAttribute key="org.eclipse.cdt.debug.gdbjtag.core.stopAt" value="handle_reset"/>\n'
        lszFile += '<stringAttribute key="org.eclipse.cdt.debug.gdbjtag.core.symbolsFileName" value=""/>\n'
        lszFile += '<stringAttribute key="org.eclipse.cdt.debug.gdbjtag.core.symbolsOffset" value=""/>\n'
        lszFile += '<booleanAttribute key="org.eclipse.cdt.debug.gdbjtag.core.useFileForImage" value="false"/>\n'
        lszFile += '<booleanAttribute key="org.eclipse.cdt.debug.gdbjtag.core.useFileForSymbols" value="false"/>\n'
        lszFile += '<booleanAttribute key="org.eclipse.cdt.debug.gdbjtag.core.useProjBinaryForImage" value="true"/>\n'
        lszFile += '<booleanAttribute key="org.eclipse.cdt.debug.gdbjtag.core.useProjBinaryForSymbols" value="true"/>\n'
        lszFile += '<booleanAttribute key="org.eclipse.cdt.debug.gdbjtag.core.useRemoteTarget" value="true"/>\n'
        lszFile += '<stringAttribute key="org.eclipse.cdt.dsf.gdb.DEBUG_NAME" value="${eclipse_home}toolchain\RISC-V Embedded GCC\\bin\\riscv-none-embed-gdb.exe"/>\n'
        lszFile += '<booleanAttribute key="org.eclipse.cdt.dsf.gdb.UPDATE_THREADLIST_ON_SUSPEND" value="false"/>\n'
        lszFile += '<intAttribute key="org.eclipse.cdt.launch.ATTR_BUILD_BEFORE_LAUNCH_ATTR" value="2"/>\n'
        lszFile += '<stringAttribute key="org.eclipse.cdt.launch.COREFILE_PATH" value=""/>\n'
        lszFile += '<stringAttribute key="org.eclipse.cdt.launch.PROGRAM_NAME" value="' + lszMRS_build_output + '"/>\n'
        lszFile += '<stringAttribute key="org.eclipse.cdt.launch.PROJECT_ATTR" value="' + lszProjectName + '"/>\n'
        lszFile += '<booleanAttribute key="org.eclipse.cdt.launch.PROJECT_BUILD_CONFIG_AUTO_ATTR" value="false"/>\n'
        lszFile += '<stringAttribute key="org.eclipse.cdt.launch.PROJECT_BUILD_CONFIG_ID_ATTR" value=""/>\n'
        lszFile += '<listAttribute key="org.eclipse.debug.core.MAPPED_RESOURCE_PATHS">\n'
        lszFile += '<listEntry value="/' + lszProjectName + '"/>\n'
        lszFile += '</listAttribute>\n'
        lszFile += '<listAttribute key="org.eclipse.debug.core.MAPPED_RESOURCE_TYPES">\n'
        lszFile += '<listEntry value="4"/>\n'
        lszFile += '</listAttribute>\n'
        lszFile += '<stringAttribute key="org.eclipse.debug.core.source_locator_id" value="org.eclipse.cdt.debug.core.sourceLocator"/>\n'
        lszFile += '<stringAttribute key="org.eclipse.debug.core.source_locator_memento" value="&lt;?xml version=&quot;1.0&quot; encoding=&quot;UTF-8&quot; standalone=&quot;no&quot;?&gt;&#13;&#10;&lt;sourceLookupDirector&gt;&#13;&#10;&lt;sourceContainers duplicates=&quot;false&quot;&gt;&#13;&#10;&lt;container memento=&quot;&amp;lt;?xml version=&amp;quot;1.0&amp;quot; encoding=&amp;quot;UTF-8&amp;quot; standalone=&amp;quot;no&amp;quot;?&amp;gt;&amp;#13;&amp;#10;&amp;lt;default/&amp;gt;&amp;#13;&amp;#10;&quot; typeId=&quot;org.eclipse.debug.core.containerType.default&quot;/&gt;&#13;&#10;&lt;container memento=&quot;&amp;lt;?xml version=&amp;quot;1.0&amp;quot; encoding=&amp;quot;UTF-8&amp;quot; standalone=&amp;quot;no&amp;quot;?&amp;gt;&amp;#13;&amp;#10;&amp;lt;mapping backend_enabled=&amp;quot;true&amp;quot; name=&amp;quot;Found Mappings&amp;quot;&amp;gt;&amp;#13;&amp;#10;&amp;lt;mapEntry memento=&amp;quot;&amp;amp;lt;?xml version=&amp;amp;quot;1.0&amp;amp;quot; encoding=&amp;amp;quot;UTF-8&amp;amp;quot; standalone=&amp;amp;quot;no&amp;amp;quot;?&amp;amp;gt;&amp;amp;#13;&amp;amp;#10;&amp;amp;lt;mapEntry backendPath=&amp;amp;quot;' + lszMRS_build_cmake + '/&amp;amp;quot; localPath=&amp;amp;quot;' + lszMRS_Dir_Main + '\&amp;amp;quot;/&amp;amp;gt;&amp;amp;#13;&amp;amp;#10;&amp;quot;/&amp;gt;&amp;#13;&amp;#10;&amp;lt;/mapping&amp;gt;&amp;#13;&amp;#10;&quot; typeId=&quot;org.eclipse.cdt.debug.core.containerType.mapping&quot;/&gt;&#13;&#10;&lt;/sourceContainers&gt;&#13;&#10;&lt;/sourceLookupDirector&gt;&#13;&#10;"/>\n'
        lszFile += '<stringAttribute key="process_factory_id" value="org.eclipse.cdt.dsf.gdb.GdbProcessFactory"/>\n'
        lszFile += '</launchConfiguration>\n'



        self.mcSystem.mcFile.CreateDirForFile(lszFileName)
        lbWrite =True
        # Check if it is already existing
        if (os.path.exists(os.path.join(lszFileName))):
            with open(os.path.join(lszFileName), "r") as loExistingGenList:
                lbWrite = False
                #lszExistingGenList = loExistingGenList.read()
                #if (lszExistingGenList != lszEmBitzPrj):
                    #lbWrite = True
        else:
            lbWrite = True

        if (lbWrite == True):
            with open(os.path.join(lszFileName), "w") as text_file:
                text_file.write(lszFile)


    def write(self, lszString):
        self.mcSystem.write(lszString)

    def debug(self, lszString):
        if (self.lbDebug == True):
            self.writeln(lszString)


    def writeln(self, lszString):
        self.mcSystem.writeln(lszString)

    def error(self, lszPrompt, liErrorNumber, lszErrorText, end='\n', lbAddTraceback = False):
        self.mcSystem.error(lszPrompt, liErrorNumber, lszErrorText, end, lbAddTraceback)

    def warn(self, lszPrompt, liWarnNumber, lszWarnText):
        self.mcSystem.warn(lszPrompt, liWarnNumber, lszWarnText)

