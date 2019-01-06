/*
	This file is part of cpp-vapory.

	cpp-vapory is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	cpp-vapory is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with cpp-vapory.  If not, see <http://www.gnu.org/licenses/>.
 */
/** @file BlockChainLoader.h
 * @author Marek Kotewicz <marek@ethdev.com>
 * @date 2015
 */

#pragma once
#include <json/json.h>
#include <libdevcore/TransientDirectory.h>
#include <libvapory/BlockChain.h>
#include <libvapory/Block.h>
#include <libvapashseal/GenesisInfo.h>

namespace dev
{
namespace test
{

/**
 * @brief Should be used to load test blockchain from json file
 * Loads the blockchain from json, creates temporary directory to store it, removes the directory on dealloc
 */
class BlockChainLoader
{
public:
	BlockChainLoader(Json::Value const& _json, vap::Network _sealEngineNetwork = vap::Network::TransitionnetTest);
	vap::BlockChain const& bc() const { return *m_bc; }
	vap::State const& state() const { return m_block.state(); }	// TODO remove?
	vap::Block const& block() const { return m_block; }

private:
	TransientDirectory m_dir;
	std::unique_ptr<vap::BlockChain> m_bc;
	vap::Block m_block;
};

}
}
