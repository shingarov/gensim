/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#include "generators/ExecutionEngine/BlockJITExecutionEngineGenerator.h"
#include "generators/BlockJIT/JitGenerator.h"
#include "arch/ArchDescription.h"
#include "isa/ISADescription.h"

using namespace gensim::generator;

BlockJITExecutionEngineGenerator::BlockJITExecutionEngineGenerator(GenerationManager &man) : EEGenerator(man, "blockjit")
{
	man.AddModuleEntry(ModuleEntry("EE", "gensim::" + man.GetArch().Name + "::BlockJITEE", "ee_blockjit.h", ModuleEntryType::ExecutionEngine));
	man.AddModuleEntry(ModuleEntry("BlockJITTranslator", "captive::arch::" + man.GetArch().Name + "::JIT", "ee_blockjit.h", ModuleEntryType::BlockJITTranslator));
	sources = {"ee_blockjit.cpp"};
}


bool BlockJITExecutionEngineGenerator::GenerateHeader(util::cppformatstream& str) const
{
	str << "#ifndef GENSIM_" << Manager.GetArch().Name << "_BLOCKJIT\n";
	str << "#define GENSIM_" << Manager.GetArch().Name << "_BLOCKJIT\n";
	str << "#include <core/execution/BlockJITExecutionEngine.h>\n";
	str << "#include \"decode.h\"\n";
	str << "#include \"arch.h\"\n";
	
	JitGenerator jitgen (Manager);
	jitgen.GenerateClass(str);
	
	str << "namespace gensim {";
	str << "namespace " << Manager.GetArch().Name << "{";
	
	str << "class BlockJITEE : public archsim::core::execution::BlockJITExecutionEngine {";
	str << "public:";
	str << "BlockJITEE();";
	str << "};";
	
	str << "}";
	str << "}";
	
	str << "#endif\n\n";
	
	return true;
}

bool BlockJITExecutionEngineGenerator::GenerateSource(util::cppformatstream& str) const
{
	str << "#include \"ee_blockjit.h\"\n";
	str << "#include \"decode.h\"\n";
	str << "#include \"arch.h\"\n";
	str << "#include <abi/devices/Device.h>\n";
	str << "#include <abi/devices/Component.h>\n";
	str << "#include <translate/jit_funs.h>\n";
	
	str << "using namespace gensim::" << Manager.GetArch().Name << ";";
	
	JitGenerator jitgen (Manager);
	jitgen.GenerateTranslation(str);
	
	str << "BlockJITEE::BlockJITEE() : archsim::core::execution::BlockJITExecutionEngine(new captive::arch::" << Manager.GetArch().Name << "::JIT) {}";
	
	
	for(auto &isa : Manager.GetArch().ISAs) {
		jitgen.GenerateHelpers(str, isa);
		
		for(auto &insn : isa->Instructions) {
			jitgen.GenerateJITFunction(str, *isa, *insn.second);
		}	
	}


	
	
	return true;
}

const std::vector<std::string> BlockJITExecutionEngineGenerator::GetSources() const
{
	return sources;
}

DEFINE_COMPONENT(BlockJITExecutionEngineGenerator, ee_blockjit);